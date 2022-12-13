#!/usr/bin/env python

import json
import os


def js_dump_dict(f, data: dict, indent: int = 8):
    f.write(" " * indent + "{")

    for k, v in data.items():
        f.write(f" {k}: ")

        if isinstance(v, dict):
            js_dump_dict(f, v, 0)
        elif isinstance(v, list):
            f.write(str(v).replace("'", '"') + ",")
        elif isinstance(v, bool):
            f.write(str(v).lower() + ",")
        elif (k == "id" and isinstance(v, str)) or not isinstance(v, str):
            f.write(f"{v},")
        else:
            f.write(f'"{v}",')

    f.write(" },")
    if indent > 0:
        f.write("\n")


def js_dump_group(f, data: dict, key: str, indent: int):
    f.write(f"    {key}: [\n")

    for i in data[key]:
        if "comment" in i:
            f.write(" " * indent + f"// {i['comment']}\n")
            continue

        js_dump_dict(f, i, indent)

    f.write("    ],\n")


def c_dump_item(f, data: dict, key: str):
    def val_or_default(item: dict, value: str):
        if value not in item:
            if value == "use":
                return "false"
            else:
                return 0
        if isinstance(item[value], bool):
            return str(item[value]).lower()
        elif isinstance(item[value], str) and item[value][:4] == "ids.":
            return "ids::" + item[value][4:]
        return item[value]

    for item in data[key]:
        if "comment" in item:
            continue
        f.write("    { ")

        if isinstance(item["id"], str):
            if "ids." in item["id"]:
                id_str = "ids::" + item["id"][4:]
            else:
                raise Exception("Bad item id")
        else:
            id_str = item["id"]

        if key == "weapon":
            slot_id = 0
        elif key == "off_hand":
            slot_id = 1
        elif key == "ranged":
            slot_id = 2
        elif key == "head":
            slot_id = 3
        elif key == "neck":
            slot_id = 4
        elif key == "shoulder":
            slot_id = 5
        elif key == "back":
            slot_id = 6
        elif key == "chest":
            slot_id = 7
        elif key == "wrist":
            slot_id = 8
        elif key == "hands":
            slot_id = 9
        elif key == "waist":
            slot_id = 10
        elif key == "legs":
            slot_id = 11
        elif key == "feet":
            slot_id = 12
        elif key == "finger":
            slot_id = 13
        elif key == "trinket":
            slot_id = 14
        else:
            raise Exception("Unrecognized gear slot " + key)

        f.write(
            f'{id_str}, {slot_id}, "{item["title"]}", '
            f'{val_or_default(item, "ilvl")}, '
            f'{val_or_default(item, "itemset")}, '
            f'{val_or_default(item, "int")}, '
            f'{val_or_default(item, "spi")}, '
            f'{val_or_default(item, "sp")}, '
            f'{val_or_default(item, "hit")}, '
            f'{val_or_default(item, "crit")}, '
            f'{val_or_default(item, "haste")}, '
            f'{val_or_default(item, "phase")}, '
            f'{val_or_default(item, "twohand")}, '
            f'{val_or_default(item, "use")}, '
        )

        if "sockets" in item:
            f.write('"')
            for sock in item["sockets"]:
                f.write(sock)
            f.write('", ')
        else:
            f.write("nullptr, ")

        f.write("{")
        if "bonus" in item:
            f.write(
                f'{val_or_default(item["bonus"], "int")}, '
                f'{val_or_default(item["bonus"], "spi")}, '
                f'{val_or_default(item["bonus"], "sp")}, '
                f'{val_or_default(item["bonus"], "hit")}, '
                f'{val_or_default(item["bonus"], "crit")}, '
                f'{val_or_default(item["bonus"], "haste")}'
            )
        else:
            f.write("0, 0, 0, 0")
        f.write("}")
        f.write(" },\n")


def c_dump_gem(f, data: dict):
    def val_or_default(item: dict, value: str):
        if value == "color":
            return f"'{item['color']}'"
        if value not in item:
            if value == "unique":
                return "false"
            else:
                return 0
        if isinstance(item[value], bool):
            return str(item[value]).lower()
        return item[value]

    if "comment" in data:
        return

    f.write("    { ")

    if isinstance(data["id"], str):
        if "ids." in data["id"]:
            id_str = "ids::" + data["id"][4:]
        else:
            raise Exception("Bad item id")
    else:
        id_str = data["id"]

    f.write(
        f'{id_str}, "{data["title"]}", '
        f'{val_or_default(data, "color")}, '
        f'{val_or_default(data, "int")}, '
        f'{val_or_default(data, "spi")}, '
        f'{val_or_default(data, "sp")}, '
        f'{val_or_default(data, "hit")}, '
        f'{val_or_default(data, "crit")}, '
        f'{val_or_default(data, "haste")}, '
        f'{val_or_default(data, "mp5")}, '
        f'{val_or_default(data, "unique")}, '
    )

    if "req" in data:
        f.write('"')
        for color, qty in data["req"].items():
            f.write(color * qty)
        f.write('"')
    else:
        f.write("nullptr")
    f.write(" },\n")


def c_dump_enchants(f, data: dict, key: str):
    def val_or_default(enchant: dict, value: str):
        if value not in enchant:
            if value == "twohand":
                return "false"
            else:
                return 0
        if isinstance(enchant[value], bool):
            return str(enchant[value]).lower()
        return enchant[value]

    if key == "weapon":
        slot = 0
    elif key == "head":
        slot = 1
    elif key == "shoulder":
        slot = 2
    elif key == "back":
        slot = 3
    elif key == "chest":
        slot = 4
    elif key == "wrist":
        slot = 5
    elif key == "hands":
        slot = 6
    elif key == "waist":
        slot = 7
    elif key == "legs":
        slot = 8
    elif key == "feet":
        slot = 9
    elif key == "finger":
        slot = 10

    for enchant in data[key]:
        if isinstance(enchant["id"], str) and "ids." in enchant["id"]:
            id_str = "ids::" + enchant["id"][4:]
        else:
            id_str = enchant["id"]

        f.write("    { ")

        f.write(
            f"{id_str}, "
            f'{val_or_default(enchant, "enchantmentId")}, '
            f'{slot}, '
            f'"{val_or_default(enchant, "title")}", '
            f'{val_or_default(enchant, "intellect")}, '
            f'{val_or_default(enchant, "spi")}, '
            f'{val_or_default(enchant, "sp")}, '
            f'{val_or_default(enchant, "crit")}, '
            f'{val_or_default(enchant, "haste")}, '
            f'{val_or_default(enchant, "mp5")}, '
            f'{val_or_default(enchant, "twohand")}'
        )

        f.write("},\n")


def c_dump_glyph(f, data: dict):
    f.write("    { ")
    f.write(f'{data["itemId"]}, {data["spellId"]}, "{data["name"]}", {data["type"]}')
    f.write(" },\n")


def c_dump_itemset(f, data: dict):
    def val_or_default(itemset: dict, value: str):
        if value not in itemset:
            return 0
        return itemset[value]

    f.write("    { ")

    if isinstance(data["id"], str) and "ids." in data["id"]:
        id_str = "ids::" + data["id"][4:]
    else:
        id_str = data["id"]

    f.write(f"{id_str}, ")

    if "set2" in data:
        f.write("{ ")
        f.write(
            f'{val_or_default(data["set2"], "sp")}, '
            f'{val_or_default(data["set2"], "hit")}, '
            f'{val_or_default(data["set2"], "crit")}, '
            f'{val_or_default(data["set2"], "haste")} '
        )
        f.write("}, ")
    else:
        f.write("{ 0, 0, 0, 0 }, ")

    if "set4" in data:
        f.write("{ ")
        f.write(
            f'{val_or_default(data["set4"], "sp")}, '
            f'{val_or_default(data["set4"], "hit")}, '
            f'{val_or_default(data["set4"], "crit")}, '
            f'{val_or_default(data["set4"], "haste")} '
        )
    else:
        f.write("{ 0, 0, 0, 0 ")

    f.write("} },\n")


def main():
    root_dir = os.path.join(os.path.dirname(__file__), "..")
    data_file = os.path.join(root_dir, "wow_data.json")
    items_js_file = os.path.join(root_dir, "assets", "js", "items.js")
    glyphs_js_file = os.path.join(root_dir, "assets", "js", "glyphs.js")
    h_file = os.path.join(root_dir, "src", "wow_data.h")

    with open(data_file, "r") as f:
        data = json.load(f)

    with open(h_file, "w", newline="") as h:
        h.write(
            "#pragma once\n\n// This code was automatically generated by wow_data_code_gen.py.\n"
            "// DO NOT EDIT!!!\n\nnamespace wow_data\n{\nenum ids\n{\n"
        )

        with open(items_js_file, "w", newline="") as js:
            js.write("var ids = {\n")

            for name, id in data["ids"].items():
                js.write(f"    {name}: {id},\n")
                h.write(f"    {name} = {id},\n")

            js.write("};\n\nvar equip = {\n")
            h.write("};\n")

            # TODO: Any other stats we care about?
            h.write(
                """
struct Bonus
{
    int intellect;
    int spirit;
    int sp;
    int hit;
    int crit;
    int haste;
};

struct Item
{
    int id;
    int slot;
    const char *title;
    int ilvl;
    int itemset;
    int intellect;
    int spirit;
    int sp;
    int hit;
    int crit;
    int haste;
    int phase;
    bool two_hand;
    bool use;
    const char *sockets;
    Bonus bonus;
};

struct Gem
{
    int id;
    const char *title;
    char color;
    int intellect;
    int spirit;
    int sp;
    int hit;
    int crit;
    int haste;
    int mp5;
    bool unique;
    const char *req;
};

struct Enchant
{
    int id;
    int enchantmentId;
    int slot;
    const char *title;
    int intellect;
    int spirit;
    int sp;
    int crit;
    int haste;
    int mp5;
    bool two_hand;
};

struct Itemset
{
    int id;
    Bonus set2;
    Bonus set4;
};

struct Glyph
{
    int itemId;
    int spellId;
    const char *name;
    int type;
};

"""
            )

            h.write("constexpr Item items[] = {\n")
            for key in [
                "weapon",
                "off_hand",
                "ranged",
                "head",
                "neck",
                "shoulder",
                "back",
                "chest",
                "wrist",
                "hands",
                "waist",
                "legs",
                "feet",
                "finger",
                "trinket",
            ]:
                js_dump_group(js, data, key, 8)
                c_dump_item(h, data, key)

            js.write("};\n\nvar gems = [\n")
            h.write("};\n\nconstexpr Gem gems[] = {\n")

            for gem in data["gems"]:
                js_dump_dict(js, gem, 4)
                c_dump_gem(h, gem)

            js.write("];\n\nvar enchants = {\n")
            h.write("};\n\nconstexpr Enchant enchants[] = {\n")

            for key in [
                "weapon",
                "head",
                "shoulder",
                "back",
                "chest",
                "wrist",
                "hands",
                "waist",
                "legs",
                "feet",
                "finger",
            ]:
                js_dump_group(js, data["enchants"], key, 8)
                c_dump_enchants(h, data["enchants"], key)

            js.write("};\n\nvar itemsets = [\n")
            h.write("};\nconstexpr Itemset itemsets[] = {\n")

            for itemset in data["itemsets"]:
                js_dump_dict(js, itemset, 4)
                c_dump_itemset(h, itemset)

            js.write(
                "];\n\nvar quicksets = {\n\n};\n\nexport default { ids, equip, gems, enchants, itemsets, quicksets };\n"
            )
            h.write("};\n")

        with open(glyphs_js_file, "w") as g:
            g.write("export default [\n")
            h.write("\nconstexpr Glyph glyphs[] = {\n")
            for glyph in data["glyphs"]:
                js_dump_dict(g, glyph, 4)
                c_dump_glyph(h, glyph)
            g.write("];\n")
            h.write("};\n")

        h.write("\n} // namespace wow_data\n")
    pass

if __name__ == "__main__":
    main()
