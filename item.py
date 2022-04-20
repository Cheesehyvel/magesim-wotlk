import sys
import requests
import argparse
import pprint
import re
import json

parser = argparse.ArgumentParser()
parser.add_argument("item_id", help="Item ID")
args = parser.parse_args()

if args.item_id[:4] == "http":
    url = args.item_id + "&xml"
else:
    url = "https://wotlk.evowow.com/?item=" + args.item_id + "&xml"

response = requests.get(url)
if response.status_code != 200:
    raise Exception("Error http code: " + response.status_code)

xml = response.content.decode()

p = re.compile("\<error\>(.*)\<\/error\>")
m = p.search(xml)
if m:
    print("Error: "+m.group(1))
    sys.exit()

stats = {}

# ID
p = re.compile("\<item id=\"([0-9]+)\"")
m = p.search(xml)
if not m:
    print("No id")
    sys.exit()
stats["id"] = int(m.group(1))

# Name
p = re.compile("\<name\>\<\!\[CDATA\[([^\]]+)\]\]")
m = p.search(xml)
if not m:
    print("No name")
    sys.exit()
stats["title"] = m.group(1)

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

# Quality
p = re.compile("\<quality id=\"([0-9]+)\"")
m = p.search(xml)
if m and m.group(1) == "3":
    stats["q"] = "rare"

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

# Convert to item string
output = json.dumps(stats)
p = re.compile("\"([a-z0-9]+)\":")
output = p.sub(r"\1:", output)
output = output[:1]+" "+output[1:]
output = output[:-1]+" "+output[-1:]
print(output)