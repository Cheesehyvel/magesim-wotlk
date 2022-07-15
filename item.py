import sys
import requests
import argparse
import pprint
import re
import json

def getItem(item_id, phase = 1):
    if item_id[:4] == "http":
        url = item_id + "&xml"
    else:
        url = "https://wotlk.evowow.com/?item=" + item_id + "&xml"

    response = requests.get(url)
    if response.status_code != 200:
        print("Error for "+item_id+" - http code: " + response.status_code)
        return None

    xml = response.content.decode()

    p = re.compile("\<error\>(.*)\<\/error\>")
    m = p.search(xml)
    if m:
        print("Error for "+item_id+" : "+m.group(1))
        return None

    stats = {}

    # ID
    p = re.compile("\<item id=\"([0-9]+)\"")
    m = p.search(xml)
    if not m:
        print("No id for "+item_id)
        return None
    stats["id"] = int(m.group(1))

    # Name
    p = re.compile("\<name\>\<\!\[CDATA\[([^\]]+)\]\]")
    m = p.search(xml)
    if not m:
        print("No name for "+item_id)
        return None
    stats["title"] = m.group(1)

    # Item level
    p = re.compile("\<level\>([0-9]+)\<")
    m = p.search(xml)
    if m:
        stats["ilvl"] = int(m.group(1))

    # json equip data
    p = re.compile("\<jsonEquip\>\<\!\[CDATA\[([^\]]+)\]\]")
    m = p.search(xml)
    if m:
        equip = json.loads("{"+m.group(1)+"}")
        if "int" in equip:
            stats["int"] = equip["int"]
        if "spi" in equip:
            stats["spi"] = equip["spi"]
        if "splpwr" in equip:
            stats["sp"] = equip["splpwr"]
        if "critstrkrtng" in equip:
            stats["crit"] = equip["critstrkrtng"]
        if "hitrtng" in equip:
            stats["hit"] = equip["hitrtng"]
        if "hastertng" in equip:
            stats["haste"] = equip["hastertng"]
        if "manargn" in equip:
            stats["mp5"] = equip["manargn"]

    # Sockets
    p = re.compile("\>([A-Z])[a-z]+ Socket\<")
    m = p.findall(xml)
    if m:
        stats["sockets"] = []
        for g in m:
            key = g[:1].lower()
            stats["sockets"].append(key)

    # Socket bonus
    p = re.compile("\>Socket Bonus: \<[^\>]+\>([^\<]+)\<")
    m = p.search(xml)
    if m:
        p = re.compile("\+([0-9]+) ([A-Za-z0-9\ ]+)")
        m = p.search(m.group(1))
        if m:
            if m.group(2) == "Spell Power":
                stats["bonus"] = {"sp": int(m.group(1))}
            if m.group(2) == "Haste Rating":
                stats["bonus"] = {"haste": int(m.group(1))}
            if m.group(2) == "Critical Strike Rating":
                stats["bonus"] = {"crit": int(m.group(1))}
            if m.group(2) == "Hit Rating":
                stats["bonus"] = {"hit": int(m.group(1))}
            if m.group(2) == "Intellect":
                stats["bonus"] = {"int": int(m.group(1))}
            if m.group(2) == "Spirit":
                stats["bonus"] = {"spi": int(m.group(1))}
            if m.group(2) == "Mana per 5 sec":
                stats["bonus"] = {"mp5": int(m.group(1))}

    # Two hand
    p = re.compile("\<inventorySlot id=\"([0-9]+)\"")
    m = p.search(xml)
    if m and m.group(1) == "17":
        stats["twohand"] = True

    # Quality
    p = re.compile("\<quality id=\"([0-9]+)\"")
    m = p.search(xml)
    if m and m.group(1) == "3":
        stats["q"] = "rare"

    # Phase
    if phase > 1:
        stats["phase"] = phase;

    # Convert to item string
    output = json.dumps(stats)
    p = re.compile("\"([a-z0-9]+)\":")
    output = p.sub(r"\1:", output)
    output = output[:1]+" "+output[1:]
    output = output[:-1]+" "+output[-1:]

    return output

parser = argparse.ArgumentParser()
parser.add_argument("item_id", help="Item ID(s)")
parser.add_argument("-p", help="Phase", type=int, default=1)
args = parser.parse_args()

ids = args.item_id.split(",")

for index, item_id in enumerate(ids):
    item = getItem(item_id, args.p)
    if item != None:
        print(item+",")