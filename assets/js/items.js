var ids = {
    BLACK_MAGIC: 59625,
    HYPERSPEED_ACCELERATORS: 54999,
    LIGHTWEAVE_EMBROIDERY: 55642,
    DARKGLOW_EMBROIDERY: 55769,
    // Metas
    META_CHAOTIC_SKYFLARE: 41285,
    META_EMBER_SKYFLARE: 41333,
    META_BEAMING_EARTHSIEGE: 41389,
    META_INSIGHTFUL_EARTHSIEGE: 41389,
    // TBC items
    SERPENT_COIL: 30720,
    MQG: 19339,
    SKULL_GULDAN: 32483,
    SHRUNKEN_HEAD: 33829,
    NAARU_SLIVER: 34429,
    ASHTONGUE_TALISMAN: 32488,
};

var equip = {
    weapon: [
        { id: 34182, title: "Grand Magister's Staff of Torrents", int: 52, sp: 266, crit: 49, hit: 50, twohand: true, sockets: ["y", "y", "y"], bonus: { sp: 5 } },
        { id: 34336, title: "Sunflare", int: 20, sp: 292, crit: 30, haste: 23 },
    ],
    off_hand: [
        { id: 34179, title: "Heart of the Pit", int: 21, sp: 39, haste: 32 },
    ],
    ranged: [
        { id: 34347, title: "Wand of the Demonsoul", int: 10, sp: 22, haste: 18, sockets: ["y"], bonus: { sp: 2 } },
    ],
    head: [
        { id: 34340, title: "Dark Conjuror's Collar", int: 42, sp: 75, crit: 38, haste: 30, sockets: ["m", "b"], bonus: { sp: 5 } },
        { id: 34847, title: "Annihilator Holo-Gogs", int: 37, sp: 81, crit: 42, sockets: ["m", "b"], bonus: { sp: 5 } },
        { id: 32525, title: "Cowl of the Illidari High Lord", int: 31, sp: 64, crit: 47, hit: 21, sockets: ["m", "b"], bonus: { sp: 5 } },
        { id: 34405, title: "Helm of Arcane Purity", int: 42, spi: 38, sp: 75, crit: 30, sockets: ["m", "r"], bonus: { sp: 5 } },
    ],
    neck: [
        { id: 34359, title: "Pendant of Sunfire", int: 19, sp: 34, crit: 25, haste: 25, sockets: ["y"], bonus: { sp: 2 } },
        { id: 34204, title: "Amulet of Unfettered Magics", int: 17, sp: 39, hit: 15, haste: 32 },
    ],
    shoulder: [
        { id: 34210, title: "Amice of the Convoker", int: 28, sp: 53, crit: 22, haste: 30, sockets: ["r", "y"], bonus: { sp: 4 } },
        { id: 34393, title: "Shoulderpads of Knowledge's Pursuit", int: 33, spi: 22, sp: 53, crit: 26, sockets: ["r", "y"], bonus: { sp: 4 } },
    ],
    back: [
        { id: 34242, title: "Tattered Cape of Antonidas", int: 26, sp: 42, haste: 32, sockets: ["r"], bonus: { sp: 2 } },
    ],
    chest: [
        { id: 34364, title: "Sunfire Robe", int: 34, sp: 71, crit: 40, haste: 40, sockets: ["r", "r", "r"], bonus: { sp: 5 } },
        { id: 34399, title: "Robes of Ghostly Hatred", int: 40, spi: 32, sp: 71, crit: 26, haste: 27, sockets: ["r", "r", "y"], bonus: { sp: 5 } },
        { id: 34232, title: "Fel Conquerer Raiments", int: 41, sp: 71, crit: 24, haste: 33, sockets: ["r", "y", "y"], bonus: { sp: 5 } },
    ],
    wrist: [
        { id: 34447, title: "Bracers of the Tempest", int: 17, spi: 14, sp: 39, crit: 11, haste: 26, sockets: ["r"], bonus: { crit: 2 } },
    ],
    hands: [
        { id: 34344, title: "Handguards of Defiled Worlds", int: 32, sp: 47, hit: 27, haste: 36, sockets: ["y", "r"], bonus: { sp: 4 } },
        { id: 34406, title: "Gloves of Tyri's Power", int: 32, spi: 27, sp: 47, haste: 36, sockets: ["r", "y"], bonus: { sp: 4 } },
        { id: 34366, title: "Sunfire Handwraps", int: 30, sp: 53, crit: 37, sockets: ["r", "r"], bonus: { sp: 4 } },
    ],
    waist: [
        { id: 34557, title: "Belt of the Tempest", int: 29, spi: 20, sp: 50, crit: 17, hit: 14, haste: 29, sockets: ["y"], bonus: { sp: 2 } },
    ],
    legs: [
        { id: 34181, title: "Leggings of Calamity", int: 41, sp: 71, crit: 33, haste: 32, sockets: ["r", "r", "y"], bonus: { sp: 5 } },
        { id: 34386, title: "Pantaloons of Growing Strife", int: 36, spi: 25, sp: 71, haste: 42, sockets: ["r", "y", "y"], bonus: { sp: 5 } },
        { id: 30916, title: "Leggings of Channeled Elements", int: 28, spi: 28, sp: 59, crit: 34, hit: 18, sockets: ["y", "y", "b"], bonus: { sp: 5 } },
    ],
    feet: [
        { id: 34574, title: "Boots of the Tempest", int: 29, spi: 20, sp: 50, crit: 20, hit: 15, haste: 25, sockets: ["y"], bonus: { sp: 2 } },
        { id: 34919, title: "Boots of Incantations", int: 26, spi: 23, sp: 47, hit: 17, sockets: ["y"], bonus: { sp: 2 } },
    ],
    finger: [
        { id: 34362, title: "Loop of Forged Power", int: 28, sp: 34, hit: 19, haste: 30 },
        { id: 34230, title: "Ring of Omnipotence", int: 14, sp: 40, crit: 22, haste: 31 },
    ],
    trinket: [
        // TBC trinkets
        { id: ids.NAARU_SLIVER, title: "Shifting Naaru Sliver", haste: 54, use: true },
        { id: ids.SKULL_GULDAN, title: "The Skull of Gul'dan", sp: 55, hit: 25, use: true },
        { id: ids.ASHTONGUE_TALISMAN, title: "Ashtongue Talisman" },
        { id: ids.SERPENT_COIL, title: "Serpent-Coil Braid", crit: 30, hit: 12 },
        { id: ids.SHRUNKEN_HEAD, title: "Hex Shrunken Head", sp: 53, use: true },
        { id: ids.MQG, title: "Mind Quickening Gem", use: true },
    ],
};

var gems = [
    { id: ids.META_CHAOTIC_SKYFLARE, title: "Chaotic Skyflare Diamond", color: "m", crit: 21, desc: "+3% crit dmg", req: {b: 2} },
    { id: ids.META_EMBER_SKYFLARE, title: "Ember Skyflare Diamond", color: "m", sp: 25, desc: "+2% int", req: {r: 3} },
    { id: ids.META_BEAMING_EARTHSIEGE, title: "Beaming Earthsiege Diamond", color: "m", crit: 21, desc: "+2% mana", req: {y: 1, r: 2} },
    { id: ids.META_INSIGHTFUL_EARTHSIEGE, title: "Insightful Earthsiege Diamond", color: "m", int: 21, desc: "Chance to restore mana", req: {b: 1, y: 1, r: 1} },
    { id: 42144, title: "Runed Dragon's Eye", color: "r", sp: 39, unique: true },
    { id: 40113, title: "Runed Cardinal Ruby", color: "r", sp: 23 },
    { id: 45883, title: "Runed Stormjewel", color: "r", sp: 23 },
    { id: 39998, title: "Runed Scarlet Ruby", color: "r", sp: 19 },
    { id: 40151, title: "Lumunous Ametrine", color: "o", sp: 12, int: 10 },
    { id: 40152, title: "Potent Ametrine", color: "o", sp: 12, crit: 10 },
    { id: 40153, title: "Veiled Ametrine", color: "o", sp: 12, hit: 10 },
    { id: 40155, title: "Reckless Ametrine", color: "o", sp: 12, haste: 10 },
    { id: 40047, title: "Lumunous Monarch Topaz", color: "o", sp: 9, int: 8 },
    { id: 40048, title: "Potent Monarch Topaz", color: "o", sp: 9, crit: 8 },
    { id: 40049, title: "Veiled Monarch Topaz", color: "o", sp: 9, hit: 8 },
    { id: 40051, title: "Reckless Monarch Topaz", color: "o", sp: 9, haste: 8 },
    { id: 42148, title: "Brilliant Dragon's Eye", color: "y", int: 34, unique: true },
    { id: 42149, title: "Smooth Dragon's Eye", color: "y", crit: 34, unique: true },
    { id: 42156, title: "Rigid Dragon's Eye", color: "y", hit: 34, unique: true },
    { id: 42150, title: "Quick Dragon's Eye", color: "y", haste: 34, unique: true },
    { id: 40123, title: "Brilliant King's Amber", color: "y", int: 20 },
    { id: 40124, title: "Smooth King's Amber", color: "y", crit: 20 },
    { id: 40125, title: "Rigid King's Amber", color: "y", hit: 20 },
    { id: 40128, title: "Quick King's Amber", color: "y", haste: 20 },
    { id: 40012, title: "Brilliant Autumn's Glow", color: "y", int: 16 },
    { id: 40013, title: "Smooth Autumn's Glow", color: "y", crit: 16 },
    { id: 40014, title: "Rigid Autumn's Glow", color: "y", hit: 16 },
    { id: 40017, title: "Quick Autumn's Glow", color: "y", haste: 16 },
    { id: 40132, title: "Glowing Dreadstone", color: "p", sp: 12 },
    { id: 40133, title: "Purified Dreadstone", color: "p", sp: 12, spi: 10 },
    { id: 40134, title: "Royal Dreadstone", color: "p", sp: 12, mp5: 5 },
    { id: 40025, title: "Glowing Twilight Opal", color: "p", sp: 9 },
    { id: 40026, title: "Purified Twilight Opal", color: "p", sp: 9, spi: 8 },
    { id: 40027, title: "Royal Twilight Opal", color: "p", sp: 9, mp5: 4 },
    { id: 40164, title: "Timeless Eye of Zul", color: "g", int: 10 },
    { id: 40165, title: "Jagged Eye of Zul", color: "g", crit: 10 },
    { id: 40166, title: "Vivid Eye of Zul", color: "g", hit: 10 },
    { id: 40169, title: "Forceful Eye of Zul", color: "g", haste: 10 },
    { id: 40170, title: "Seer's Eye of Zul", color: "g", int: 10, spi: 10 },
    { id: 40171, title: "Misty Eye of Zul", color: "g", crit: 10, spi: 10 },
    { id: 40172, title: "Shining Eye of Zul", color: "g", hit: 10, spi: 10 },
    { id: 40174, title: "Intricate Eye of Zul", color: "g", haste: 10, spi: 10 },
    { id: 40175, title: "Dazzling Eye of Zul", color: "g", int: 10, mp5: 5 },
    { id: 40176, title: "Sundered Eye of Zul", color: "g", crit: 10, mp5: 5 },
    { id: 40177, title: "Lambent Eye of Zul", color: "g", hit: 10, mp5: 5 },
    { id: 40179, title: "Energized Eye of Zul", color: "g", haste: 10, mp5: 5 },
    { id: 40085, title: "Timeless Forest Emerald", color: "g", int: 8 },
    { id: 40086, title: "Jagged Forest Emerald", color: "g", crit: 8 },
    { id: 40088, title: "Vivid Forest Emerald", color: "g", hit: 8 },
    { id: 40091, title: "Forceful Forest Emerald", color: "g", haste: 8 },
    { id: 40092, title: "Seer's Forest Emerald", color: "g", int: 8, spi: 8 },
    { id: 40095, title: "Misty Forest Emerald", color: "g", crit: 8, spi: 8 },
    { id: 40099, title: "Shining Forest Emerald", color: "g", hit: 8, spi: 8 },
    { id: 40104, title: "Intricate Forest Emerald", color: "g", haste: 8, spi: 8 },
    { id: 40094, title: "Dazzling Forest Emerald", color: "g", int: 8, mp5: 4 },
    { id: 40096, title: "Sundered Forest Emerald", color: "g", crit: 8, mp5: 4 },
    { id: 40100, title: "Lambent Forest Emerald", color: "g", hit: 8, mp5: 4 },
    { id: 40105, title: "Energized Forest Emerald", color: "g", haste: 8, mp5: 4 },
    { id: 42145, title: "Sparkling Dragon's Eye", color: "b", spi: 34, unique: true },
    { id: 42146, title: "Lustrous Dragon's Eye", color: "b", mp5: 17, unique: true },
    { id: 40120, title: "Sparkling Majestic Zircon", color: "b", spi: 20 },
    { id: 40121, title: "Lustrous Majestic Zircon", color: "b", mp5: 10 },
    { id: 40009, title: "Sparkling Sky Sapphire", color: "b", spi: 16 },
    { id: 40010, title: "Lustrous Sky Sapphire", color: "b", mp5: 8 },
];

var enchants = {
    weapon: [
        { id: 62948, title: "Greater Spellpower", sp: 81, twohand: true },
        { id: 60714, title: "Mighty Spellpower", sp: 63 },
        { id: 59619, title: "Accuracy", crit: 25, hit: 25 },
        { id: ids.BLACK_MAGIC, title: "Black Magic" }, // 35% chance, 35s icd
    ],
    head: [
        { id: 59970, title: "Arcanum of Burning Mysteries", sp: 30, crit: 20 },
        { id: 59960, title: "Arcanum of Blissful Mending", sp: 30, mp5: 10 },
    ],
    shoulder: [
        { id: 59937, title: "Greater Inscription of the Storm ", sp: 24, crit: 15 },
        { id: 59936, title: "Greater Inscription of the Crag", sp: 24, mp5: 8 },
    ],
    back: [
        { id: ids.LIGHTWEAVE_EMBROIDERY, title: "Lightweave Embroidery" },
        { id: ids.DARKGLOW_EMBROIDERY, title: "Darkglow Embroidery" },
        { id: 47898, title: "Greater Speed", haste: 23 },
        { id: 47899, title: "Wisdom", spi: 10 },
    ],
    chest: [
        { id: 60692, title: "Powerful Stats", int: 10, spi: 10 },
        { id: 44509, title: "Greater Mana Restoration", mp5: 10 },
    ],
    wrist: [
        { id: 60767, title: "Superior Spellpower", sp: 30 },
        { id: 44593, title: "Major spirit", spi: 18 },
        { id: 44555, title: "Exceptional Intellect", int: 16 },
    ],
    hands: [
        { id: ids.HYPERSPEED_ACCELERATORS, title: "Hyperspeed Accelerators" },
        { id: 44592, title: "Exceptional Spellpower", sp: 28 },
        { id: 44488, title: "Precision", hit: 20 },
    ],
    legs: [
        { id: 55631, title: "Brilliant Spellthread", sp: 50, spi: 20 },
        { id: 55634, title: "Sapphire Spellthread", sp: 50 },
        { id: 55630, title: "Shining Spellthread", sp: 35, spi: 12 },
        { id: 55632, title: "Azure Spellthread", sp: 35 },
    ],
    feet: [
        { id: 47901, title: "Tuskarr's Vitality" },
        { id: 44508, title: "Greater Spirit", spi: 18 },
        { id: 44584, title: "Greater Vitality", mp5: 7 },
        { id: 60623, title: "Icewalker", hit: 12, crit: 12 },
    ],
    finger: [
        { id: 44636, title: "Greater Spellpower", sp: 23 },
    ],
};

var itemsets = [

];

var quicksets = {

};

export default { ids, equip, gems, enchants, itemsets, quicksets };
