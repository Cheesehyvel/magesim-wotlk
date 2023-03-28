<template>
    <div id="app">
        <div class="notice notice-alt" v-if="donation_open" @click="donation_open = false">
            <div class="inner">
                <div class="title">Thank you!</div>
                <div class="text mt-2">
                    For your donation.
                </div>
            </div>
        </div>

        <div class="notices">
            <div class="error-notice notice" v-if="error_notice.open" @click="error_notice.open = false">
                <div class="inner">
                    <div class="title mb-2" v-if="error_notice.title">{{ error_notice.title }}</div>
                    <div class="text" v-if="error_notice.text">
                        <div v-for="text in error_notice.text">{{ text }}</div>
                    </div>
                </div>
            </div>
            <div class="profile-status notice" v-if="profile_status.open" @click="profile_status.open = false">
                <div class="inner">
                    <div class="title">Profile loaded</div>
                    <div class="checklist">
                        <check-item :value="profile_status.items">Items</check-item>
                        <check-item :value="false" v-for="slot in profile_status.missing_items" :key="slot">{{ formatKey(slot) }}</check-item>
                        <check-item :value="profile_status.config">Config</check-item>
                    </div>
                </div>
            </div>
        </div>

        <div class="wrapper">
            <div class="sidebar">
                <div class="actions">
                    <div class="btn large block mt-n" @click="runMultiple" :class="[is_running ? 'disabled' : '']">
                        <div>Run</div>
                        <div>{{ config.iterations }} iterations</div>
                    </div>
                    <div class="btn block mt-n" @click="runSingle" :class="[is_running ? 'disabled' : '']">
                        <div>Single iteration</div>
                    </div>
                    <div class="btn block mt-n" @click="runEP" :class="[is_running && !is_running_ep ? 'disabled' : '']">
                        <div v-if="!is_running_ep">Stat weights</div>
                        <div v-else>Stop</div>
                    </div>
                </div>
                <div class="display-stats" v-if="display_stats">
                    <table class="simple">
                        <tbody>
                            <tr>
                                <td>Mana</td>
                                <td>{{ display_stats.mana }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.intellect ? 'active' : '']">
                                    Intellect
                                    <span v-if="config.custom_stats.intellect">
                                        ({{ $plusMinus(config.custom_stats.intellect) }})
                                        <tooltip position="t">{{ config.custom_stats.intellect }} bonus intellect</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.intellect }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.spirit ? 'active' : '']">
                                    Spirit
                                    <span v-if="config.custom_stats.spirit">
                                        ({{ $plusMinus(config.custom_stats.spirit) }})
                                        <tooltip position="t">{{ config.custom_stats.spirit }} bonus spirit</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.spirit }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.mp5 ? 'active' : '']">
                                    Mp5
                                    <span v-if="config.custom_stats.mp5">
                                        ({{ $plusMinus(config.custom_stats.mp5) }})
                                        <tooltip position="t">{{ config.custom_stats.mp5 }} bonus mp5</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.mp5 }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.spell_power ? 'active' : '']">
                                    Spell power
                                    <span v-if="config.custom_stats.spell_power">
                                        ({{ $plusMinus(config.custom_stats.spell_power) }})
                                        <tooltip position="t">{{ config.custom_stats.spell_power }} bonus spell power</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.spell_power }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.crit_rating ? 'active' : '']">
                                    Crit
                                    <span v-if="config.custom_stats.crit_rating">
                                        ({{ $plusMinus(config.custom_stats.crit_rating) }})
                                        <tooltip position="t">{{ config.custom_stats.crit_rating }} bonus crit rating</tooltip>
                                    </span>
                                </td>
                                <td>
                                    <span>{{ $round(display_stats.crit, 2) }}%</span>
                                    <tooltip position="r">{{ display_stats.crit_rating }} crit rating</tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.hit_rating ? 'active' : '']">
                                    Hit
                                    <span v-if="config.custom_stats.hit_rating">
                                        ({{ $plusMinus(config.custom_stats.hit_rating) }})
                                        <tooltip position="t">{{ config.custom_stats.hit_rating }} bonus hit rating</tooltip>
                                    </span>
                                </td>
                                <td>
                                    <span>{{ $round(display_stats.hit, 2) }}%</span>
                                    <tooltip position="r">
                                        <div class="tal">
                                            <div>{{ $round(hitRatingToChance(display_stats.hit_rating),2) }}% from {{ display_stats.hit_rating }} hit rating</div>
                                            <div v-if="config.talents.precision">{{ config.talents.precision }}% from Precision</div>
                                            <div v-if="config.talents.precision && config.rotation == rotations.ROTATION_ST_FROSTFIRE">{{ config.talents.precision }}% extra from Precision (ffb only, not included)</div>
                                            <div v-if="config.debuff_spell_hit">3% from spell hit debuff</div>
                                            <div v-if="config.heroic_presence && faction == 'alliance'">1% from Heroic Presence</div>
                                            <div v-if="config.talents.arcane_focus">3% from Arcane Focus (arcane only, not included)</div>
                                        </div>
                                    </tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.haste_rating ? 'active' : '']">
                                    Haste
                                    <span v-if="config.custom_stats.haste_rating">
                                        ({{ $plusMinus(config.custom_stats.haste_rating) }})
                                        <tooltip position="t">{{ config.custom_stats.haste_rating }} bonus haste rating</tooltip>
                                    </span>
                                </td>
                                <td>
                                    <span>{{ $round(display_stats.haste, 2) }}%</span>
                                    <tooltip position="r">{{ display_stats.haste_rating }} haste rating</tooltip>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                    <div class="mt-1 tac">
                        <div class="btn small" @click="openCustomStats">
                            Add bonus stats
                        </div>
                    </div>
                </div>
                <div class="ep-stats" v-if="epCalc">
                    <div class="title">
                        <span>Stat weights</span>
                        <help>
                            Stat weights are calculated by running {{ config.iterations }} iterations with +{{ config.stat_weight_increment }} of each stat with the same RNG seed and comparing the dps gain.<br>
                            Calculated stat weights are based on your config. Any changes to it or your items can change the weights.<br>
                            The best way to find out if an item/gem/enchant is better is to equip it and run simulations.
                        </help>
                    </div>
                    <select v-model="ep_weight">
                        <option value="dps">DPS</option>
                        <option value="intellect">Intellect (EP)</option>
                        <option value="spirit">Spirit (EP)</option>
                        <option value="mp5">Mp5 (EP)</option>
                        <option value="spell_power">Spell power (EP)</option>
                        <option value="crit_rating">Crit rating (EP)</option>
                        <option value="hit_rating">Hit rating (EP)</option>
                        <option value="haste_rating">Haste rating (EP)</option>
                    </select>
                    <table class="simple mt-1">
                        <tbody>
                            <tr @click="ep_weight = 'intellect'">
                                <td>Intellect</td>
                                <td>{{ $nullRound(epCalc.intellect, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'spirit'">
                                <td>Spirit</td>
                                <td>{{ $nullRound(epCalc.spirit, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'mp5'">
                                <td>Mp5</td>
                                <td>{{ $nullRound(epCalc.mp5, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'spell_power'">
                                <td>Spell power</td>
                                <td>{{ $nullRound(epCalc.spell_power, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'crit_rating'">
                                <td>Crit rating</td>
                                <td>{{ $nullRound(epCalc.crit_rating, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'hit_rating'">
                                <td>Hit rating</td>
                                <td>{{ $nullRound(epCalc.hit_rating, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'haste_rating'">
                                <td>Haste rating</td>
                                <td>{{ $nullRound(epCalc.haste_rating, 2) }}</td>
                            </tr>
                        </tbody>
                    </table>
                    <loader class="small mt-2" v-if="is_running" />
                </div>
                <div class="no-result mt-4" v-else-if="is_running">
                    <loader v-if="is_running" />
                </div>
                <div class="result" v-else-if="result">
                    <template v-if="result.iterations">
                        <div class="dps-result">
                            <div>DPS</div>
                            <div class="faded">{{ $round(result.min_dps, 2) }} - {{ $round(result.max_dps, 2) }}</div>
                            <div class="dps">{{ $round(result.avg_dps, 2) }}</div>
                            <div class="faded mb-1" v-if="result.t_gcd_capped">
                                <span>Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s</span>
                                <help>Time spent gcd capped</help>
                            </div>
                        </div>
                        <div class="pinned" v-if="pin_dps">
                            <span class="update" @click="updatePin">
                                <span class="material-icons">&#xe5d5;</span>
                                <tooltip position="r">Update reference</tooltip>
                            </span>
                            <span class="diff" :class="[pin_dps > result.avg_dps ? 'lt' : 'gt']">
                                <template v-if="pin_dps <= result.avg_dps">+</template>{{ $roundFixed(result.avg_dps - pin_dps, 2) }}
                            </span>
                            <span class="remove" @click="removePin">
                                <span class="material-icons">&#xe5cd;</span>
                                <tooltip position="r">Remove reference</tooltip>
                            </span>
                        </div>
                        <div class="pin" v-else>
                            <span>
                                <span class="material-icons" @click="updatePin">&#xf10d;</span>
                                <tooltip position="r">Set reference</tooltip>
                            </span>
                        </div>
                        <div class="btn mt-2" v-if="!config.rng_seed" :class="[is_running ? 'disabled' : '']" @click="findAvg(result.avg_dps)">Find average fight</div>
                        <div class="btn mt-1" v-if="result.all_results" @click="allResults()">Simulation data</div>
                    </template>
                    <template v-else>
                        <div class="dps-result">
                            <div>DPS</div>
                            <div class="faded">Damage: {{ result.dmg }}</div>
                            <div class="dps">{{ $round(result.dps, 2) }}</div>
                            <div class="faded" v-if="result.t_gcd_capped">
                                <span>Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s</span>
                                <help>Time spent gcd capped</help>
                            </div>
                        </div>
                    </template>

                    <div class="warnings mt-2">
                        <div class="warning" v-if="!isMetaGemActive()">
                            <span class="material-icons">&#xe002;</span>
                            <tooltip position="right">Meta gem requirements have not been met.</tooltip>
                        </div>
                        <div class="warning" v-if="numProfs() > 2">
                            <span class="material-icons">&#xe002;</span>
                            <tooltip position="right">You have selected bonuses from {{ numProfs() }} professions</tooltip>
                        </div>
                        <div class="warning" v-if="numDragonsEye() > 3">
                            <span class="material-icons">&#xe002;</span>
                            <tooltip position="right">You have selected more than 3 Dragon's Eye gems</tooltip>
                        </div>
                    </div>
                </div>
                <a class="github" href="https://github.com/Cheesehyvel/magesim-wotlk" target="_blank"></a>
                <div class="donate">
                    <a href="https://www.paypal.com/donate/?hosted_button_id=CU9RF4LCMW8W6" target="_blank">
                        Donate
                    </a>
                </div>
            </div>
            <div class="main">
                <div class="tabs">
                    <div class="tab" :class="{active: active_tab == 'gear'}" @click="setTab('gear')">Gear</div>
                    <div class="tab" :class="{active: active_tab == 'config'}" @click="setTab('config')">Config</div>
                    <template v-if="history.length">
                        <div class="tab" :class="{active: active_tab == 'history'}" @click="setTab('history')">History</div>
                    </template>
                    <template v-if="result && !result.iterations">
                        <div class="tab" :class="{active: active_tab == 'log'}" @click="setTab('log')">Combat log</div>
                        <div class="tab" :class="{active: active_tab == 'timeline'}" @click="setTab('timeline')">Timeline</div>
                        <div class="tab" :class="{active: active_tab == 'spells'}" @click="setTab('spells')">Spells</div>
                    </template>
                    <template v-if="result && result.iterations">
                        <div class="tab" :class="{active: active_tab == 'histogram'}" @click="setTab('histogram')">Histogram</div>
                    </template>
                </div>
                <div class="body">
                    <div class="gear" :class="{splitview}" v-if="active_tab == 'gear'">
                        <div class="slots">
                            <div
                                class="slot"
                                :class="[active_slot == slot ? 'active' : '']"
                                v-for="slot in slots"
                                @click="setActiveSlot(slot);"
                            >{{ formatKey(slot) }}</div>
                            <div class="btn btn-splitview" @click="toggleSplitview">
                                <template v-if="splitview">
                                    <span>Paperdoll</span>
                                    <span class="material-icons">&#xe5e1;</span>
                                </template>
                                <template v-else>
                                    <span class="material-icons">&#xe5e0;</span>
                                    <span>Paperdoll</span>
                                </template>
                            </div>
                        </div>
                        <div class="gear-wrapper">
                            <div class="items" ref="items">
                                <div class="items-wrapper">
                                    <div class="top clearfix">
                                        <div class="fl clearfix">
                                            <div class="form-item text-search">
                                                <input type="text" ref="search" v-model="search_item" placeholder="Search..." @input="refreshTooltips">
                                                <tooltip position="bl">
                                                    <b>Text search with filters</b><br><br>
                                                    Yes/no filters: pvp, 2h, set<br>
                                                    Example: 'pvp:no' will exclude all pvp items.<br><br>
                                                    Number filters: phase, ilvl, sp, crit, hit, haste, int, spi, mp5<br>
                                                    Example: 'ilvl:252+' will find items with ilvl 252 or higher.<br>
                                                    Example: 'ilvl:252-' will find items with ilvl 252 or lower.<br>
                                                    Example: 'ilvl:239-252' will find items with ilvl between 239 and 252.
                                                </tooltip>
                                            </div>
                                        </div>
                                        <div class="fr">
                                            <div class="btn" :class="[!hasComparisons || is_running ? 'disabled' : '']" @click="runComparison">
                                                Run item comparison
                                            </div>
                                            <div class="btn ml-n" @click="openEquiplist">
                                                Equipped items overview
                                            </div>
                                            <div class="btn ml-n" @click="openCustomItem">
                                                Add custom item
                                            </div>
                                        </div>
                                    </div>

                                    <table class="items-table large mt-2">
                                        <thead>
                                            <tr>
                                                <th class="min">
                                                    <span class="compare" @click.stop="compareAll()" v-if="activeItems">
                                                        <help icon="e915">Compare all items</help>
                                                    </span>
                                                </th>
                                                <th class="min"></th>
                                                <th class="title">
                                                    <sort-link v-model="item_sort" name="title">Name</sort-link>
                                                </th>
                                                <th v-if="hasComparisons">
                                                    <sort-link v-model="item_sort" name="dps" order="desc">DPS</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="ilvl" order="desc">ilvl</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="phase">{{ splitviewShort("Phase", "P") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="sockets" order="desc">Sockets</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="sp" order="desc">{{ splitviewShort("Spell power", "SP") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="crit" order="desc">{{ splitviewShort("Crit rating", "Crit") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="hit" order="desc">{{ splitviewShort("Hit rating", "Hit") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="haste" order="desc">{{ splitviewShort("Haste rating", "Haste") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="int" order="desc">{{ splitviewShort("Intellect", "Int") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="spi" order="desc">{{ splitviewShort("Spirit", "Spi") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="mp5" order="desc">Mp5</sort-link>
                                                </th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr
                                                class="item"
                                                :class="[isEquipped(active_slot, item.id) ? 'active' : '']"
                                                v-for="item in activeItems"
                                                @click="equipToggle(active_slot, item)"
                                                :key="item.id"
                                            >
                                                <td class="min">
                                                    <span class="compare" :class="[isComparing(item) ? 'active' : '']" @click.stop="compareItem(item)">
                                                        <help icon="e915">Add to comparison</help>
                                                    </span>
                                                </td>
                                                <td class="min">
                                                    <span class="favorite" :class="[isFavorite(item) ? 'active' : '']" @click.stop="toggleFavorite(item)">
                                                        <help icon="e87d" v-if="isFavorite(item)">Favorite</help>
                                                        <help icon="e87d" :outlined="true" v-else>Favorite</help>
                                                    </span>
                                                </td>
                                                <td class="title">
                                                    <a :href="itemUrl(item)" :class="['quality-'+$get(item, 'q', 'epic')]" target="_blank" @click.prevent>
                                                        {{ item.title }}
                                                    </a>
                                                    <span class="link" @click.stop="openItem(item)">
                                                        <span class="material-icons">
                                                            &#xe895;
                                                        </span>
                                                    </span>
                                                    <span class="delete" @click.stop="deleteCustomItem(item)" v-if="$get(item, 'custom')">
                                                        <help icon="e872">Delete custom item</help>
                                                    </span>
                                                </td>
                                                <td v-if="hasComparisons">
                                                    <template v-if="comparisonDps(item)">
                                                        {{ comparisonDps(item) }}
                                                        <span v-if="pin_dps" class="diff" :class="[pin_dps > comparisonDps(item) ? 'lt' : 'gt']">
                                                            (<template v-if="pin_dps <= comparisonDps(item)">+</template>{{ $roundFixed(comparisonDps(item) - pin_dps, 2) }})
                                                        </span>
                                                        <span v-if="comparisonMetaGemInactive(item)" class="warning">
                                                            <span class="material-icons">&#xe002;</span>
                                                            <tooltip position="right">Meta gem requirements have not been met.</tooltip>
                                                        </span>
                                                    </template>
                                                </td>
                                                <td>{{ $get(item, "ilvl", "") }}</td>
                                                <td>{{ $get(item, "phase", 1) }}</td>
                                                <td @click.prevent.stop="scrollToGems">
                                                    <template v-if="item.sockets">
                                                        <div class="socket-color" :class="['color-'+socket]" v-for="socket in item.sockets"></div>
                                                    </template>
                                                    <span class="ml-n" v-if="item.bonus" :class="[hasSocketBonus(active_slot) ? 'socket-bonus' : '']">
                                                        +{{ formatStats(item.bonus) }}
                                                    </span>
                                                </td>
                                                <td>{{ $get(item, "sp", "") }}</td>
                                                <td>{{ $get(item, "crit", "") }}</td>
                                                <td>{{ $get(item, "hit", "") }}</td>
                                                <td>{{ $get(item, "haste", "") }}</td>
                                                <td>{{ $get(item, "int", "") }}</td>
                                                <td>{{ $get(item, "spi", "") }}</td>
                                                <td>{{ $get(item, "mp5", "") }}</td>
                                            </tr>
                                        </tbody>
                                    </table>

                                    <div class="mt-4" ref="enchantAnchor"></div>

                                    <table class="enchants-table large" v-if="activeEnchants.length">
                                        <thead>
                                            <tr>
                                                <th>Enchant</th>
                                                <th>Spell power</th>
                                                <th>Crit rating</th>
                                                <th>Hit rating</th>
                                                <th>Haste rating</th>
                                                <th>Intellect</th>
                                                <th>Spirit</th>
                                                <th>Mp5</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr
                                                class="item"
                                                :class="[isEnchanted(active_slot, item.id) ? 'active' : '']"
                                                v-for="item in activeEnchants"
                                                :key="item.id"
                                                @click="enchant(active_slot, item)"
                                            >
                                                <td>
                                                    <a :href="spellUrl(item)" :class="['quality-'+$get(item, 'q', 'uncommon')]" target="_blank" @click.stop>
                                                        {{ item.title }}
                                                    </a>
                                                </td>
                                                <td>{{ $get(item, "sp", "") }}</td>
                                                <td>{{ $get(item, "crit", "") }}</td>
                                                <td>{{ $get(item, "hit", "") }}</td>
                                                <td>{{ $get(item, "haste", "") }}</td>
                                                <td>{{ $get(item, "int", "") }}</td>
                                                <td>{{ $get(item, "spi", "") }}</td>
                                                <td>{{ $get(item, "mp5", "") }}</td>
                                            </tr>
                                        </tbody>
                                    </table>

                                    <div class="mt-4" ref="gemsAnchor"></div>

                                    <div class="extra-socket mb-2" v-if="['hands', 'wrist'].indexOf(active_slot) != -1">
                                        <label>
                                            <input type="checkbox" v-model="config.hands_socket" v-if="active_slot == 'hands'">
                                            <input type="checkbox" v-model="config.wrist_socket" v-if="active_slot == 'wrist'">
                                            <span>Extra socket from blacksmithing</span>
                                        </label>
                                    </div>
                                    <div class="extra-socket mb-2" v-if="active_slot == 'waist'">
                                        <label>
                                            <input type="checkbox" v-model="config.waist_socket">
                                            <span>Extra socket from <a :href="itemUrl(41611)" target="_blank">Eternal Belt Buckle</a></span>
                                        </label>
                                    </div>

                                    <div class="search-gem mb-2" v-if="activeSockets.length">
                                        <input type="text" v-model="search_gem" placeholder="Search...">
                                    </div>

                                    <div class="sockets" v-if="activeSockets.length">
                                        <div class="socket" v-for="(socket, index) in activeSockets">
                                            <table class="large">
                                                <thead>
                                                    <tr>
                                                        <th class="min">
                                                            <span class="socket-color" :class="['color-'+socket]"></span>
                                                        </th>
                                                        <th class="min narrow"></th>
                                                        <th>Gem</th>
                                                        <th>Stats</th>
                                                    </tr>
                                                </thead>
                                                <tbody>
                                                    <tr
                                                        :class="[isSocketed(active_slot, gem.id, index) ? 'active' : '']"
                                                        v-for="gem in activeGems(index)"
                                                        @click="setSocket(active_slot, gem, index)"
                                                        :key="gem.id"
                                                    >
                                                        <td class="min">
                                                            <span class="socket-color" :class="['color-'+gem.color]"></span>
                                                        </td>
                                                        <td class="min narrow">
                                                            <span class="favorite" :class="[isFavorite(gem) ? 'active' : '']" @click.stop="toggleFavorite(gem)">
                                                                <help icon="e87d" v-if="isFavorite(gem)">Favorite</help>
                                                                <help icon="e87d" :outlined="true" v-else>Favorite</help>
                                                            </span>
                                                        </td>
                                                        <td>
                                                            <a :href="itemUrl(gem)" class="gem-color" :class="['color-'+gem.color]" target="_blank" @click.stop>
                                                                {{ gem.title }}
                                                            </a>
                                                        </td>
                                                        <td>{{ formatStats(gem) }}</td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div class="character" v-if="splitview">
                                <div class="paperdoll">
                                    <div :class="pos" v-for="pos in ['left', 'right']">
                                        <div class="paperslot" :class="[slot, active_slot == slot ? 'active' : '']" v-for="slot in dollSlots(pos)">
                                            <div class="paperv paperitem" @click="paperClick(slot)">
                                                <a
                                                    v-if="equipped[slot]"
                                                    :href="equippedUrl(slot)"
                                                    data-wh-icon-size="large"
                                                    @click="$event.preventDefault()"
                                                ></a>
                                            </div>
                                            <div class="papers">
                                                <div class="paperv paperenchant" v-if="items.enchants.hasOwnProperty(equipSlotToItemSlot(slot)) && pos == 'left'" @click="paperClick(slot, 'enchant')">
                                                    <a
                                                        v-if="enchants[slot]"
                                                        :href="spellUrl(enchants[slot])"
                                                        data-wh-icon-size="large"
                                                        @click="$event.preventDefault()"
                                                    ></a>
                                                </div>
                                                <div class="paperv papersocket" :class="['papersocket-color-'+socket]" v-for="(socket, index) in slotSockets(slot)" @click="paperClick(slot, 'gems')">
                                                    <a
                                                        v-if="gems[slot][index]"
                                                        :href="itemUrl(gems[slot][index])"
                                                        data-wh-icon-size="large"
                                                        @click="$event.preventDefault()"
                                                    ></a>
                                                </div>
                                                <div class="paperv paperenchant" v-if="items.enchants.hasOwnProperty(equipSlotToItemSlot(slot)) && pos == 'right'" @click="paperClick(slot, 'enchant')">
                                                    <a
                                                        v-if="enchants[slot]"
                                                        :href="spellUrl(enchants[slot])"
                                                        data-wh-icon-size="large"
                                                        @click="$event.preventDefault()"
                                                    ></a>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="log" v-if="active_tab == 'log'">
                        <div class="log-wrapper">
                            <div class="filter">
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[1]"> <span>Show cast start</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[2]"> <span>Show cast success</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[3]"> <span>Show spell impact</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[4]"> <span>Show mana gain</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[5]"> <span>Show buffs</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[8]"> <span>Show wait</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter_player"> <span>Show player only</span></label>
                                </div>
                            </div>
                            <div class="form-item mb-2">
                                <input type="text" v-model="search_log" placeholder="Search...">
                            </div>
                            <table>
                                <thead>
                                    <th>Time</th>
                                    <th>Mana</th>
                                    <th>DPS</th>
                                    <th>Event</th>
                                </thead>
                                <tbody>
                                    <tr v-for="log in activeLog" :class="['type-'+log.type]">
                                        <td>{{ formatTime(log.t) }}</td>
                                        <td>{{ round(log.mana) }} ({{ round(log.mana_percent) }}%)</td>
                                        <td>{{ (log.t ? round(log.dmg/log.t) : "0") }}</td>
                                        <td>{{ log.text }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="timel" v-if="active_tab == 'timeline'">
                        <timeline ref="timeline" :result="result"></timeline>
                    </div>

                    <div class="spells" v-if="active_tab == 'spells'">
                        <div class="spells-wrapper">
                            <table class="large">
                                <thead>
                                    <th>Caster</th>
                                    <th>Spell</th>
                                    <th>Casts</th>
                                    <th>Misses</th>
                                    <th>Hits</th>
                                    <th>Crits</th>
                                    <th>Damage</th>
                                    <th>Min dmg</th>
                                    <th>Avg dmg</th>
                                    <th>Max dmg</th>
                                </thead>
                                <tbody>
                                    <tr v-for="spell in spellStats">
                                        <td>{{ spell.source }}</td>
                                        <td>{{ spell.name }}</td>
                                        <td>{{ spell.casts }} ({{ $round(spell.casts / numCasts * 100, 1) }}%)</td>
                                        <td>{{ spell.misses }} ({{ $round(spell.misses/(spell.hits + spell.crits + spell.misses)*100, 2) }}%)</td>
                                        <td>{{ spell.hits }}</td>
                                        <td>{{ spell.crits }} ({{ $round(spell.crits/(spell.hits + spell.crits + spell.misses)*100, 2) }}%)</td>
                                        <td>{{ $round(spell.dmg, 0) }} ({{ $round(spell.dmg / result.dmg * 100, 2) }}%)</td>
                                        <td>{{ $round(spell.min_dmg, 0) }}</td>
                                        <td>{{ $round(spell.dmg / (spell.hits + spell.crits), 0) }}</td>
                                        <td>{{ $round(spell.max_dmg, 0) }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="histog" v-if="active_tab == 'histogram'">
                        <histogram ref="histogram" :data="result.histogram" :avg="result.avg_dps"></histogram>
                    </div>

                    <div class="history" v-if="active_tab == 'history'">
                        <div class="history-wrapper">
                            <table class="history-table large">
                                <thead>
                                    <th></th>
                                    <th>DPS</th>
                                    <th>Min/Max</th>
                                    <th>Rotation</th>
                                    <th>Duration</th>
                                    <th>Iterations</th>
                                    <th>Execution time</th>
                                    <th>Time</th>
                                    <th></th>
                                </thead>
                                <tbody>
                                    <tr v-for="profile in history">
                                        <td>
                                            <div class="btn small my-n" @click="loadHistory(profile)">Load profile</div>
                                        </td>
                                        <td>
                                            <template v-if="profile.result">
                                                <b>{{ $round(profile.result.avg_dps, 2) }}</b>
                                            </template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            <template v-if="profile.result">
                                                {{ $round(profile.result.min_dps) }} - {{ $round(profile.result.max_dps) }}
                                            </template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            {{ getRotationString(profile.config.rotation) }}
                                        </td>
                                        <td>
                                            {{ profile.config.duration }}
                                            <span class="faded" v-if="profile.config.duration_variance"> &#177;{{ profile.config.duration_variance }}</span>
                                        </td>
                                        <td>{{ profile.config.iterations }}</td>
                                        <td>
                                            <template v-if="profile.end">{{ formatTimeDiff(profile.date, profile.end) }}</template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            {{ formatDateTime(profile.date) }}
                                        </td>
                                        <td>
                                            <span v-if="profile.result && profile.result.all_results" class="btn small my-n" @click="allResults(profile.result)">
                                                Simulation data
                                            </span>
                                        </td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="config" v-if="active_tab == 'config'">
                        <div class="fieldsets">
                            <fieldset class="config-general">
                                <legend>General</legend>
                                <div class="form-item">
                                    <label>Quick spec</label>
                                    <span class="btn secondary" @click="setSpec('arcane')">Arcane</span>
                                    <span class="btn secondary" @click="setSpec('fire')">Fire</span>
                                    <span class="btn secondary" @click="setSpec('frost')">Frost</span>
                                    <span class="btn secondary" @click="setSpec('ffb')">FFB</span>
                                </div>
                                <div class="form-item">
                                    <label>Race</label>
                                    <select v-model="config.race">
                                        <option :value="races.RACE_BLOOD_ELF">Blood elf</option>
                                        <option :value="races.RACE_DRAENEI">Draenei</option>
                                        <option :value="races.RACE_GNOME">Gnome</option>
                                        <option :value="races.RACE_HUMAN">Human</option>
                                        <option :value="races.RACE_TROLL">Troll</option>
                                        <option :value="races.RACE_UNDEAD">Undead</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Talents (<a :href="config.build" target="_blank">link</a>)</label>
                                    <input type="text" v-model="config.build" @input="onBuildInput">
                                </div>
                                <div class="form-item">
                                    <label>Number of sims</label>
                                    <input type="text" v-model.number="config.iterations">
                                </div>
                                <div class="form-item form-row">
                                    <div class="form-item">
                                        <label>Fight duration (sec)</label>
                                        <input type="text" v-model.number="config.duration">
                                    </div>
                                    <div class="form-item">
                                        <label>Variance +/-</label>
                                        <input type="text" v-model.number="config.duration_variance">
                                    </div>
                                </div>
                                <div class="form-item">
                                    <label>Target level</label>
                                    <select v-model="config.target_level">
                                        <option :value="83">83</option>
                                        <option :value="82">82</option>
                                        <option :value="81">81</option>
                                        <option :value="80">80</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>No. of targets</label>
                                    <input type="text" v-model.number="config.targets">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Distance from target</span>
                                        <help>This only affects travel time.<br>No range checks are made.</help>
                                    </label>
                                    <input type="text" v-model.number="config.distance">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Reaction time (ms)</span>
                                        <help>
                                            This only affects certain mechanics where an instant reaction can have an important impact on the rotation, like:<br>
                                            Cooldowns with "wait for"<br>
                                            Missile Barrage<br>
                                            Hot Streak<br>
                                            Brain Freeze
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.reaction_time">
                                </div>
                                <div class="form-item" v-if="false">
                                    <label>
                                        <span>Reaction time (ms)</span>
                                        <help>Affects cooldown usage when waiting for procs</help>
                                    </label>
                                    <input type="text" v-model.number="config.reaction_time">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>RNG seed</span>
                                        <help>
                                            A number above 0 will give all runs the same random seed.<br>
                                            All iterations in the same run will still have different seeds.<br>
                                            This might be useful for certain analysis.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.rng_seed">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Stat weight increment</span>
                                        <help>
                                            Each stat will be increased by this value when calculating stat weights.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.stat_weight_increment">
                                </div>
                                <div class="form-item">
                                    <label>Design</label>
                                    <select @input="onDesignInput($event)">
                                        <option :value="0">- Choose -</option>
                                        <option :value="1">Wotlk</option>
                                        <option :value="2">Alternative</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="config.targets > 1">
                                    <label><input type="checkbox" v-model="config.only_main_dmg">
                                        <span>Show dps for main target only</span>
                                        <help>
                                            This will ignore damage done to secondary targets in the summary.<br>
                                            This is only applicable if the numbers of targets is more than 1.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.avg_spell_dmg">
                                        <span>Use average spell damage</span>
                                        <help>
                                            This will eliminate the random damage from spells.<br>
                                            This can be useful to verify calculations.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.additional_data">
                                        <span>Additional data</span>
                                        <help>
                                            This will save data about dps and duration for each simulation.<br>
                                            This will use more memory and can cause performance issues with a high number of sims.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.encounters">
                                        <span>Enable encounters</span>
                                        <help>
                                            This will enable certain buffs/debuffs from encounters.<br>
                                            They can be found under cooldowns.
                                        </help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-rotation">
                                <legend>Rotation</legend>
                                <div class="form-item">
                                    <label>Main rotation</label>
                                    <select v-model="config.rotation">
                                        <option :value="rotations.ROTATION_ST_FROSTFIRE">Frostfire Bolt</option>
                                        <option :value="rotations.ROTATION_ST_AB_AM">Arcane</option>
                                        <option :value="rotations.ROTATION_ST_AB_AM_BARRAGE" v-if="config.talents.arcane_barrage">Arcane + Barrage</option>
                                        <option :value="rotations.ROTATION_ST_FIRE">Fire</option>
                                        <option :value="rotations.ROTATION_ST_FIRE_SC">Fire (Scorch)</option>
                                        <option :value="rotations.ROTATION_ST_FROST">Frost</option>
                                        <option :value="rotations.ROTATION_AOE_AE">Arcane Explosion</option>
                                        <option :value="rotations.ROTATION_AOE_BLIZZ">Blizzard</option>
                                        <option :value="rotations.ROTATION_AOE_BLIZZ_FS">Flamestrike > Blizzard</option>
                                        <option :value="rotations.ROTATION_AOE_FIRE">Fire AoE</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="config.talents.imp_scorch">
                                    <label><input type="checkbox" v-model="config.maintain_imp_scorch">
                                        <span>Keep up imp. scorch</span>
                                        <help>Imp. Scorch from you</help>
                                    </label>
                                </div>
                                <template v-if="[rotations.ROTATION_ST_AB_AM, rotations.ROTATION_ST_AB_AM_BARRAGE].indexOf(config.rotation) != -1">
                                    <div class="form-item">
                                        <label>Stack Arcane Blast to 3 below mana %</label>
                                        <input type="text" v-model.number="config.rot_ab3_mana">
                                    </div>
                                    <div class="form-item">
                                        <label>AB without Missile Barrage above mana %</label>
                                        <input type="text" v-model.number="config.rot_ab_no_mb_mana">
                                    </div>
                                    <div class="form-item">
                                        <label>Extra ABs during first AP</label>
                                        <input type="text" v-model.number="config.rot_abs_ap">
                                    </div>
                                    <div class="form-item">
                                        <label>
                                            <span>Use Missile Barrage below n AB stacks</span>
                                            <help>Settings this to 1 or 2 can potentially be a dps increase with Arcane Barrage rotation or T8 4p.</help>
                                        </label>
                                        <input type="text" v-model.number="config.rot_mb_below_ab">
                                    </div>
                                    <div class="form-item">
                                        <label>
                                            <span>Use Missile Barrage ASAP below mana %</span>
                                            <help>This can be useful to conserve mana</help>
                                        </label>
                                        <input type="text" v-model.number="config.rot_mb_mana">
                                    </div>
                                </template>
                                <template v-if="config.rotation == rotations.ROTATION_ST_FROST">
                                    <div class="form-item">
                                        <label><input type="checkbox" v-model="config.rot_ice_lance">
                                            <span>Ice Lance at end of Fingers of Frost</span>
                                        </label>
                                    </div>
                                    <div class="form-item">
                                        <label><input type="checkbox" v-model="config.rot_brain_freeze_fireball">
                                            <span>Fireball with Brain Freeze</span>
                                            <help>Instead of Frostfire Bolt, which is the default</help>
                                        </label>
                                    </div>
                                    <div class="form-item" v-if="config.talents.brain_freeze">
                                        <label>
                                            <span>Brain Freeze duration cutoff</span>
                                            <help>
                                                Brain Freeze will always be used when the buff duration is below the given number of seconds.<br>
                                                Otherwise it will only be used as the bonus spell at the end of Fingers of Frost.<br>
                                                Setting this to a low number can be a dps increase when you are close to haste cap.
                                            </help>
                                        </label>
                                        <input type="text" v-model.number="config.rot_brain_freeze_hold">
                                    </div>
                                </template>
                                <div class="form-item" v-if="config.talents.ignite">
                                    <label><input type="checkbox" v-model="config.ignite_munching">
                                        <span>Ignite munching</span>
                                        <help>
                                            When two spells crit at the same time, only the latter spell will count towards ignite.<br>
                                            For example when an instant pyroblast lands right after a fireball, or when Living Bomb explodes at the same time as another spell lands on the target.<br>
                                            However, this does not affect Hot Streak with Frostfire Bolt due to Frostfire Bolt having a faster travel time.
                                        </help>
                                    </label>
                                </div>
                                <template v-if="config.talents.hot_streak && config.talents.ignite && config.ignite_munching && config.rotation == rotations.ROTATION_ST_FIRE">
                                    <div class="form-item">
                                        <label><input type="checkbox" v-model="config.hot_streak_cqs">
                                            <span>Delay Pyroblast slightly</span>
                                            <help>
                                                This will delay Pyroblast by {{ config.hot_streak_cqs_time }}ms after a Hot Streak to prevent ignite munching.<br>
                                                There is no way to do this perfectly in-game, but a cqs macro can do this with about 70-90% reliability.
                                            </help>
                                        </label>
                                    </div>
                                    <div class="form-item" v-if="config.hot_streak_cqs">
                                        <label>
                                            <span>Pyroblast delay (ms)</span>
                                            <help>
                                                A delay of 10ms or less will cause ignite munching in the sim.<br>
                                                In game this is more complicated. Sometimes you get munching with 60ms, sometimes you don't with 0ms.
                                            </help>
                                        </label>
                                        <input type="text" v-model.number="config.hot_streak_cqs_time">
                                    </div>
                                </template>
                                <div class="form-item">
                                    <label>
                                        <span>Number of Evocation ticks</span>
                                        <help>
                                            This will allow you to cancel evocation early.<br>
                                            Setting this to 0 will cast a full evocation.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.evo_ticks">
                                </div>
                                <div class="form-item" v-if="canBlackMagicWeave">
                                    <label><input type="checkbox" v-model="config.rot_black_magic">
                                        <span>Black Magic weaving</span>
                                        <help>
                                            This will swap your weapon with an identical weapon with Black Magic enchant, and then swap back after it procs.<br>
                                            The swaps will only happen after an instant cast spell to avoid a gcd.
                                        </help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-debuffs">
                                <legend>Debuffs</legend>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.debuff_crit" :disabled="config.totem_of_wrath">
                                        <span>3% crit</span>
                                        <help>
                                            Heart of the Crusader<br>Master Poisoner<br>Totem of Wrath
                                            <br><br>If you have Totem of Wrath you should select it under buffs instead.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.debuff_spell_crit">
                                        <span>5% spell crit</span>
                                        <help>
                                            Imp. Shadow Bolt<br>Winter's Chill<br>Imp. Scorch
                                            <br><br>Do no check this if you are the person keeping up Winter's Chill or Imp. Scorch.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.debuff_spell_hit">
                                        <span>3% spell hit</span>
                                        <help>Misery<br>Imp. Faerie Fire</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.debuff_spell_dmg">
                                        <span>13% spell dmg</span>
                                        <help>Curse of Elements<br>Earth and Moon<br>Ebon Spellbringer</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.judgement_of_wisdom">
                                        <span>Judgement of Wisdom</span>
                                        <help>Chance to restore 2% base mana on spell hit</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.gift_of_arthas">
                                        <span>Gift of Arthas</span>
                                        <help>Acts as +8 spell power debuff on target</help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-buffs">
                                <legend>Buffs</legend>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.mage_armor" @input="dontStack($event, 'molten_armor')"> <span>Mage Armor</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.molten_armor" @input="dontStack($event, 'mage_armor')"> <span>Molten Armor</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" :checked="true" :disabled="true">
                                        <span>Arcane Intellect</span>
                                        <help>60 intellect</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.divine_spirit" @input="dontStack($event, 'fel_intelligence')">
                                        <span>Divine Spirit</span>
                                        <help>80 spirit</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.fel_intelligence" @input="dontStack($event, 'divine_spirit')">
                                        <span>Fel intelligence</span>
                                        <help>64 spirit<br>The intellect part does not stack with Arcane Intellect</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.mark_of_the_wild">
                                        <span>Mark of the Wild</span>
                                        <help>37 stats</help>
                                    </label>
                                </div>
                                <div class="form-item sub" v-if="config.mark_of_the_wild">
                                    <label><input type="checkbox" v-model="config.imp_mark_of_the_wild">
                                        <span class="material-icons">&#xe5da;</span>
                                        <span>Imp. Mark of the Wild</span>
                                        <help>52 stats instead of 37</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.totem_of_wrath" @input="config.debuff_crit = $event.target.checked; dontStack($event, 'flametongue')">
                                        <span>Totem of Wrath</span>
                                        <help>3% crit + 280 spell power</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.flametongue" @input="dontStack($event, 'totem_of_wrath')">
                                        <span>Flametongue Totem</span>
                                        <help>144 spell power</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.demonic_pact">
                                        <span>Demonic Pact</span>
                                        <help>Does not stack with Totem of Wrath or Flametongue totem.</help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.demonic_pact">
                                    <label>
                                        <span>Demonic Pact Bonus</span>
                                        <help>10% of the Warlocks spell power.</help>
                                    </label>
                                    <input type="text" v-model.number="config.demonic_pact_bonus">
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.buff_spell_haste">
                                        <span>Wrath of Air Totem</span>
                                        <help>5% spell haste</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.mana_spring" @input="dontStack($event, 'blessing_of_wisdom')">
                                        <span>Mana Spring Totem</span>
                                        <help>91 mp5</help>
                                    </label>
                                </div>
                                <div class="form-item sub" v-if="config.mana_spring">
                                    <label><input type="checkbox" v-model="config.restorative_totems">
                                        <span class="material-icons">&#xe5da;</span>
                                        <span>Restorative Totems</span>
                                        <help>109 mp5 instead of 91</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.blessing_of_wisdom" @input="dontStack($event, 'mana_spring')">
                                        <span>Blessing of Wisdom</span>
                                        <help>92 mp5</help>
                                    </label>
                                </div>
                                <div class="form-item sub" v-if="config.blessing_of_wisdom">
                                    <label><input type="checkbox" v-model="config.imp_blessing_of_wisdom">
                                        <span class="material-icons">&#xe5da;</span>
                                        <span>Imp. Blessing of Wisdom</span>
                                        <help>110 mp5 instead of 91</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.blessing_of_kings" @input="dontStack($event, 'drums_of_forgotten_kings')">
                                        <span>Blessing of Kings</span>
                                        <help>10% stats</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.drums_of_forgotten_kings" @input="dontStack($event, 'blessing_of_kings')">
                                        <span>Drums of Forgotten Kings</span>
                                        <help>8% stats</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.buff_dmg">
                                        <span>3% damage</span>
                                        <help>Sanctified Retribution<br>Ferocious Inspiration<br>Arcane Empowerment</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.buff_spell_crit">
                                        <span>5% spell crit</span>
                                        <help>Moonkin Aura<br>Elemental Oath</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.buff_haste">
                                        <span>3% haste</span>
                                        <help>Improved moonkin form<br>Swift Retribution</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.mana_replenishment">
                                        <span>Replenish 1% mana every 5 sec</span>
                                        <help>Vampiric Touch<br>Judgement of the Wise<br>Hunting Party<br>Improved Soul Leech<br>Enduring Winter</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.focus_magic">
                                        <span>Focus Magic</span>
                                        <help>
                                            3% spell crit, put on you by another mage.<br>
                                            If you have Focus Magic talented it will be factored in with permanent uptime after 5 seconds into the fight.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="faction == 'alliance'">
                                    <label><input type="checkbox" v-model="config.heroic_presence">
                                        <span>Heroic Presence</span>
                                        <help>1% hit from Draenei Racial.<br>This is automatically applied if your race is Draenei</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.prof_skinning">
                                        <span>Master of Anatomy (Skinning)</span>
                                        <help>40 crit rating</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.prof_alchemy">
                                        <span>Mixology (Alchemy)</span>
                                        <help>
                                            Bonuses to flask and elixirs.<br>
                                            Flask of the Frostwyrm: +47sp<br>
                                            Spellpower Elixir: +23sp<br>
                                            Guru's Elixir: +8 stats<br>
                                            Elixir of Accuracy: +20 hit rating<br>
                                            Elixir of Deadly Strikes: +20 crit rating<br>
                                            Elixir of Lightning Speed: +20 haste rating<br>
                                            Elixir of Spirit: +20 spirit<br>
                                            Elixir of Mighty Mageblood: +10 mp5<br>
                                            Elixir of Mighty Thoughts: +20 int
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.prof_engineer">
                                        <span>Mana Injectors (Engineering)</span>
                                        <help>25% extra from mana potions</help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-consumes">
                                <legend>Consumes</legend>
                                <div class="form-item" v-if="!config.battle_elixir && !config.guardian_elixir">
                                    <label>Flask</label>
                                    <select v-model="config.flask">
                                        <option :value="flasks.FLASK_NONE">None</option>
                                        <option :value="flasks.FLASK_FROSTWYRM">Flask of the Frostwyrm (125 sp)</option>
                                        <option :value="flasks.FLASK_PURE_MOJO">Flask of Pure Mojo (45 mp5)</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="!config.flask">
                                    <label>Battle Elixir</label>
                                    <select v-model="config.battle_elixir">
                                        <option :value="elixirs.ELIXIR_NONE">None</option>
                                        <option :value="elixirs.ELIXIR_SPELLPOWER">Spellpower Elixir (58 sp)</option>
                                        <option :value="elixirs.ELIXIR_GURU">Guru's Elixir (20 stats)</option>
                                        <option :value="elixirs.ELIXIR_ACCURACY">Elixir of Accuracy (45 hit)</option>
                                        <option :value="elixirs.ELIXIR_DEADLY_STRIKES">Elixir of Deadly Strikes (45 crit)</option>
                                        <option :value="elixirs.ELIXIR_LIGHTNING_SPEED">Elixir of Lightning Speed (45 haste)</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="!config.flask">
                                    <label>Guardian Elixir</label>
                                    <select v-model="config.guardian_elixir">
                                        <option :value="elixirs.ELIXIR_NONE">None</option>
                                        <option :value="elixirs.ELIXIR_SPIRIT">Elixir of Spirit (50 spirit)</option>
                                        <option :value="elixirs.ELIXIR_MIGHTY_MAGEBLOOD">Elixir of Mighty Mageblood (30 mp5)</option>
                                        <option :value="elixirs.ELIXIR_MIGHTY_THOUGHTS">Elixir of Mighty Thoughts (45 int)</option>
                                        <option :value="elixirs.ELIXIR_DRAENIC_WISDOM">Elixir of Draenic Wisdom (30 int, 30 spirit)</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Food</label>
                                    <select v-model="config.food">
                                        <option :value="foods.FOOD_NONE">None</option>
                                        <option :value="foods.FOOD_SPELL_POWER">Firecracker Salmon (46 sp)</option>
                                        <option :value="foods.FOOD_CRIT">Spiced Worm Burger (40 crit)</option>
                                        <option :value="foods.FOOD_HIT">Snapper Extreme (40 hit)</option>
                                        <option :value="foods.FOOD_HASTE">Imperial Manta Steak (40 haste)</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Potion</label>
                                    <select v-model="config.potion">
                                        <option :value="potions.POTION_NONE">None</option>
                                        <option :value="potions.POTION_MANA">Mana potion</option>
                                        <option :value="potions.POTION_SPEED">Potion of Speed</option>
                                        <option :value="potions.POTION_WILD_MAGIC">Potion of Wild Magic</option>
                                        <option :value="potions.POTION_FLAME_CAP">Flame Cap</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Conjured</label>
                                    <select v-model="config.conjured">
                                        <option :value="conjureds.CONJURED_NONE">None</option>
                                        <option :value="conjureds.CONJURED_DARK_RUNE">Dark Rune</option>
                                    </select>
                                </div>
                            </fieldset>
                            <fieldset class="config-precombat">
                                <legend>Pre-combat</legend>
                                <div class="form-item">
                                    <label>Potion</label>
                                    <select v-model="config.pre_potion">
                                        <option :value="potions.POTION_NONE">None</option>
                                        <option :value="potions.POTION_MANA">Mana potion</option>
                                        <option :value="potions.POTION_SPEED">Potion of Speed</option>
                                        <option :value="potions.POTION_WILD_MAGIC">Potion of Wild Magic</option>
                                        <option :value="potions.POTION_FLAME_CAP">Flame Cap</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.pre_mirror_image">
                                        <span>Mirror Image</span>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.talents.water_elemental">
                                    <label><input type="checkbox" v-model="config.pre_water_elemental">
                                        <span>Water Elemental</span>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.talents.incanters_absorption">
                                    <label><input type="checkbox" v-model="config.pre_incanters_absorption">
                                        <span>Fire Ward + Sapper</span>
                                        <help>Sapper will be popped with Arcane Power if no timing is specified.</help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.talents.incanters_absorption && config.pre_incanters_absorption">
                                    <label><input type="checkbox" v-model="config.pre_mana_incanters_absorption">
                                        <span>Mana Shield</span>
                                        <help>
                                            Mana Shield can absorb damage from Sapper Charge and/or Dark Rune.<br>
                                            If you want to use a Dark Rune, make sure you select it as the Conjured consumable.<br>
                                            Dark Rune will be popped with Arcane Power if no timing is specified.
                                        </help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-cooldowns">
                                <legend>Cooldowns</legend>
                                <div class="timings">
                                    <table class="items">
                                        <thead>
                                            <tr>
                                                <th class="icon">CD</th>
                                                <th class="t">Pop at</th>
                                                <th class="wait-for-buff">Wait for</th>
                                                <th class="wait-t">Wait max</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr v-for="timing in config.timings" :key="timing.id">
                                                <template v-if="timingEnabled(timing.name)">
                                                    <td class="icon">
                                                        <span>
                                                            <img :src="getTiming(timing.name, 'icon')">
                                                            <tooltip>{{ getTiming(timing.name, 'title') }}</tooltip>
                                                        </span>
                                                        <div class="remove" @click="removeTiming(timing.id)">
                                                            <span class="material-icons">&#xe5cd;</span>
                                                        </div>
                                                    </td>
                                                    <td class="t">
                                                        <input type="text" v-model.number="timing.t">
                                                    </td>
                                                    <td class="wait-for-buff">
                                                        <select v-model="timing.wait_for_buff" v-if="timingCanWait(timing.name)">
                                                            <option :value="0">Nothing</option>
                                                            <option v-for="buff in waitBuffs" :value="buff.id" :key="buff.id">{{ buff.name }}</option>
                                                        </select>
                                                    </td>
                                                    <td class="wait-t">
                                                        <input type="text" v-model.number="timing.wait_t" v-if="timingCanWait(timing.name)">
                                                    </td>
                                                </template>
                                            </tr>
                                        </tbody>
                                    </table>
                                    <div class="add-timing">
                                        <div class="plus">
                                            <span class="material-icons">&#xe145;</span>
                                        </div>
                                        <div class="menu">
                                            <template v-for="timing in timings">
                                                <div v-if="timingEnabled(timing.name)" @click="addTiming(timing.name)">
                                                    <img :src="timing.icon">
                                                    <tooltip>{{ timing.title }}</tooltip>
                                                </div>
                                            </template>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-interruptions">
                                <legend>Interruptions</legend>
                                <div class="interruptions">
                                    <table class="items">
                                        <thead>
                                            <tr>
                                                <th class="type">Type</th>
                                                <th class="affects">Affects</th>
                                                <th class="t">Time</th>
                                                <th class="duration">Duration</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr v-for="interruption in config.interruptions" :key="interruption.id">
                                                <td class="type">
                                                    <select v-model="interruption.silence">
                                                        <option :value="false">Movement</option>
                                                        <option :value="true">Silence</option>
                                                    </select>
                                                    <div class="remove" @click="removeInterruption(interruption.id)">
                                                        <span class="material-icons">&#xe5cd;</span>
                                                    </div>
                                                </td>
                                                <td class="affects">
                                                    <select v-model="interruption.affects_all">
                                                        <option :value="false">Player</option>
                                                        <option :value="true">Player and pets</option>
                                                    </select>
                                                </td>
                                                <td class="t">
                                                    <input type="text" v-model.number="interruption.t">
                                                </td>
                                                <td class="t">
                                                    <input type="text" v-model.number="interruption.duration">
                                                </td>
                                            </tr>
                                        </tbody>
                                    </table>
                                    <div class="add-timing">
                                        <div class="plus" @click="addInterruption">
                                            <span class="material-icons">&#xe145;</span>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-profiles">
                                <legend>Your profiles</legend>
                                <div class="profiles">
                                    <div class="profile" v-for="(profile, index) in profiles" :key="profile.id">
                                        <div class="name" @click="loadProfile(profile)">{{ profile.name }}</div>
                                        <div class="actions">
                                            <div class="move move-up" @click="moveProfile(index, -1)">
                                                <span class="material-icons">&#xe316;</span>
                                                <tooltip position="t">Move up</tooltip>
                                            </div>
                                            <div class="move move-down" @click="moveProfile(index, 1)">
                                                <span class="material-icons">&#xe313;</span>
                                                <tooltip position="t">Move down</tooltip>
                                            </div>
                                            <div class="load-items" @click="loadProfile(profile, 'items')">
                                                <span class="material-icons">&#xe84e;</span>
                                                <tooltip position="t">Load items only</tooltip>
                                            </div>
                                            <div class="load-config" @click="loadProfile(profile, 'config')">
                                                <span class="material-icons">&#xe8b8;</span>
                                                <tooltip position="t">Load config only</tooltip>
                                            </div>
                                            <div class="save" @click="saveProfile(profile)">
                                                <span class="material-icons">&#xe161;</span>
                                                <tooltip position="t">Save profile</tooltip>
                                            </div>
                                            <div class="delete" @click="deleteProfile(profile)">
                                                <span class="material-icons">&#xe872;</span>
                                                <tooltip position="t">Delete profile</tooltip>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="new-profile mt-1">
                                        <input type="text" v-model="new_profile" placeholder="Enter the name of your new profile" @keydown.enter="newProfile()">
                                        <div class="btn" :class="[new_profile ? '' : 'disabled']" @click="newProfile()">
                                            <span>
                                                New profile
                                                <tooltip position="r">Save your items and config</tooltip>
                                            </span>
                                        </div>
                                    </div>
                                    <div class="export-import clearfix mt-2">
                                        <div class="btn fl" @click="openExport()">Export</div>
                                        <div class="btn fl ml-n" @click="openImport()">Import</div>
                                        <div class="btn danger fr" @click="nukeSettings()">Nuke settings</div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-quick-profiles">
                                <legend>Quick profiles</legend>
                                <div class="profiles">
                                    <div class="profile" v-for="(profile, index) in default_profiles" :key="profile.id">
                                        <div class="name" @click="loadProfile(profile)">{{ profile.name }}</div>
                                    </div>
                                </div>
                            </fieldset>
                        </div>
                    </div>
                </div>
            </div>

            <div class="lightbox" v-if="export_profile.open">
                <div class="closer" @click="closeExport"></div>
                <div class="inner">
                    <div class="title">Export</div>
                    <div class="form-item">
                        <textarea v-model="export_profile.string" ref="export_input"></textarea>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="export_profile.items" @input="updateExport"> <span>Include items</span></label>
                        <label><input type="checkbox" v-model="export_profile.config" @input="updateExport"> <span>Include config</span></label>
                    </div>
                    <div class="btn mt-2 wide" @click="closeExport">Close</div>
                </div>
            </div>

            <div class="lightbox" v-if="import_profile.open">
                <div class="closer" @click="closeImport"></div>
                <div class="inner">
                    <div class="title">Import</div>
                    <div class="description">
                        Supported formats: MageSim, EightyUpgrades, WowSims Exporter addon.
                    </div>
                    <div class="form-item">
                        <textarea v-model="import_profile.string" ref="import_input" @input="checkImportString"></textarea>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="import_profile.items" :disabled="!import_status.items"> <span>Include items</span></label>
                        <label><input type="checkbox" v-model="import_profile.config" :disabled="!import_status.config"> <span>Include config</span></label>
                    </div>
                    <div class="btn mt-2 wide" :class="[import_profile.string ? '' : 'disabled']" @click="doImport">Import</div>
                </div>
            </div>

            <div class="lightbox" v-if="equiplist_open">
                <div class="closer" @click="closeEquiplist"></div>
                <div class="inner">
                    <div class="title">Equipped items</div>
                    <table>
                        <thead>
                            <tr>
                                <th>Slot</th>
                                <th>Item</th>
                                <th>Enchant</th>
                                <th>Gems</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr class="equipped-item" v-for="(item_id, slot) in equipped">
                                <template v-if="item_id">
                                    <td>{{ formatKey(slot) }}</td>
                                    <td>
                                        <a :href="itemUrl(item_id)" target="_blank" :class="['quality-'+$get(getItem(slot, item_id), 'q', 'epic')]">
                                            {{ getItem(slot, item_id).title }}
                                        </a>
                                    </td>
                                    <td>
                                        <template v-if="$get(enchants, slot)">
                                            <a :href="spellUrl(enchants[slot])" target="_blank" :class="['quality-'+$get(getEnchant(slot, enchants[slot]), 'q', 'uncommon')]">
                                                {{ getEnchant(slot, enchants[slot]).title }}
                                            </a>
                                        </template>
                                    </td>
                                    <td>
                                        <template v-for="(gem_id, index) in gems[slot]" v-if="gems.hasOwnProperty(slot)">
                                            <template v-if="gem_id">
                                                <span v-if="index > 0">, </span>
                                                <a :href="itemUrl(gem_id)" target="_blank" :class="['gem-color', 'color-'+getGem(gem_id).color]">
                                                    {{ getGem(gem_id).title }}
                                                </a>
                                            </template>
                                        </template>
                                    </td>
                                </template>
                            </tr>
                        </tbody>
                    </table>
                    <div class="mt-2">
                        <div class="btn wide" @click="copyEquiplist">Copy</div>
                    </div>
                </div>
            </div>

            <div class="lightbox small" v-if="custom_stats_open">
                <div class="closer" @click="closeCustomStats"></div>
                <div class="inner">
                    <div class="title">Bonus stats</div>
                    <div class="description">Add additional stats to your character.</div>
                    <div class="form-item form-row">
                        <label>Intellect</label>
                        <input type="number" v-model.number="config.custom_stats.intellect">
                    </div>
                    <div class="form-item form-row">
                        <label>Spirit</label>
                        <input type="number" v-model.number="config.custom_stats.spirit">
                    </div>
                    <div class="form-item form-row">
                        <label>Spell Power</label>
                        <input type="number" v-model.number="config.custom_stats.spell_power">
                    </div>
                    <div class="form-item form-row">
                        <label>Crit rating</label>
                        <input type="number" v-model.number="config.custom_stats.crit_rating">
                    </div>
                    <div class="form-item form-row">
                        <label>Hit rating</label>
                        <input type="number" v-model.number="config.custom_stats.hit_rating">
                    </div>
                    <div class="form-item form-row">
                        <label>Haste rating</label>
                        <input type="number" v-model.number="config.custom_stats.haste_rating">
                    </div>
                    <div class="form-item form-row">
                        <label>Mp5</label>
                        <input type="number" v-model.number="config.custom_stats.mp5">
                    </div>
                </div>
            </div>

            <div class="lightbox small" v-if="custom_item_open">
                <div class="closer" @click="closeCustomItem"></div>
                <div class="inner">
                    <div class="title">Add custom item</div>
                    <div class="description">Custom items will only be added for your browser.</div>
                    <div class="form">
                        <div class="form-item form-row">
                            <label>
                                ID
                                <help>Leave empty to generate a random ID</help>
                            </label>
                            <input type="number" v-model.number="custom_item.id">
                        </div>
                        <div class="form-item form-row">
                            <label>Name</label>
                            <input type="text" v-model="custom_item.title">
                        </div>
                        <div class="form-item form-row">
                            <label>Slot</label>
                            <select v-model="custom_item.slot">
                                <option :value="null">- Choose -</option>
                                <option :value="slot" v-for="slot in itemSlots">{{ formatKey(slot) }}</option>
                            </select>
                        </div>
                        <div class="form-item form-row" v-if="custom_item.slot == 'weapon'">
                            <label>Twohanded</label>
                            <div>
                                <input type="checkbox" v-model="custom_item.twohand">
                            </div>
                        </div>
                        <div class="form-item form-row">
                            <label>Quality</label>
                            <select v-model="custom_item.q">
                                <option value="epic">Epic</option>
                                <option value="rare">Rare</option>
                                <option value="uncommon">Uncommon</option>
                                <option value="common">Common</option>
                            </select>
                        </div>
                        <div class="form-item form-row">
                            <label>Intellect</label>
                            <input type="number" v-model.number="custom_item.int">
                        </div>
                        <div class="form-item form-row">
                            <label>Spirit</label>
                            <input type="number" v-model.number="custom_item.spi">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power</label>
                            <input type="number" v-model.number="custom_item.sp">
                        </div>
                        <div class="form-item form-row">
                            <label>Crit rating</label>
                            <input type="number" v-model.number="custom_item.crit">
                        </div>
                        <div class="form-item form-row">
                            <label>Hit rating</label>
                            <input type="number" v-model.number="custom_item.hit">
                        </div>
                        <div class="form-item form-row">
                            <label>Haste rating</label>
                            <input type="number" v-model.number="custom_item.haste">
                        </div>
                        <div class="form-item form-row">
                            <label>Mp5</label>
                            <input type="number" v-model.number="custom_item.mp5">
                        </div>
                        <div class="form-item form-row">
                            <label>Number of prismatic sockets</label>
                            <input type="number" v-model.number="custom_item.sockets">
                        </div>
                        <div class="form-item form-row" v-if="custom_item.slot == 'head'">
                            <label>Meta socket</label>
                            <div>
                                <input type="checkbox" v-model="custom_item.meta_socket">
                            </div>
                        </div>
                    </div>
                    <div class="mt-2 text-error" v-if="custom_item_error">
                        {{ custom_item_error }}
                    </div>
                    <div class="mt-2">
                        <div class="btn block" @click="addCustomItem">Save</div>
                    </div>
                </div>
            </div>

            <div class="lightbox small warning" v-if="quick_start_open">
                <div class="inner">
                    <div class="title">Quick start</div>
                    <div class="text">
                        Select a profile below to start your simming!<br>
                        <span class="faded">You can access all of these profiles later and even create your own under the</span> <b>Config</b> <span class="faded">tab.</span>
                    </div>
                    <div class="profile-choices">
                        <div class="profile-choice btn" v-for="profile in default_profiles" @click="loadProfile(profile); quick_start_open = false">
                            <img :src="profile.icon" v-if="profile.icon">
                            {{ profile.name }}
                        </div>
                    </div>
                    <div class="btn mt-2" @click="quick_start_open = false">
                        No thanks, I'll start from scratch
                    </div>
                </div>
            </div>

        </div>
    </div>
</template>

<script>
    import { SimulationWorker, SimulationWorkers } from "./simulation";
    import items from "./items";
    import glyphs from "./glyphs";
    import constants from "./constants";
    import default_profiles from "./default_profiles";
    const DEFAULT_DESIGN = 2;

    export default {
        created() {
            var design = localStorage.getItem("design");
            if (!design)
                design = DEFAULT_DESIGN;
            document.documentElement.classList.add("design-"+design);

            this.checkNewUser();
        },

        mounted() {
            this.loadCustomItems();
            this.loadFavorites();
            this.loadCurrentProfile();
            this.loadDefaultProfiles();
            this.loadProfiles();
            this.calcStats();
            this.checkDonation();
        },

        data() {
            var splitview = localStorage.getItem("splitview") == "true" ? true : false;

            var default_config = {
                iterations: 20000,
                race: 5,
                shatt_faction: 0,

                duration: 180,
                duration_variance: 0,
                rng_seed: 0,
                stat_weight_increment: 20,
                only_main_dmg: false,
                avg_spell_dmg: false,
                additional_data: false,
                targets: 1,
                target_resistance: 0,
                target_level: 83,
                distance: 20,
                reaction_time: 300,
                encounters: false,

                // Debuffs
                debuff_crit: false,
                debuff_spell_crit: false,
                debuff_spell_hit: false,
                debuff_spell_dmg: false,
                judgement_of_wisdom: false,
                gift_of_arthas: false,

                // Buffs
                mage_armor: false,
                molten_armor: true,
                amplify_magic: false,
                divine_spirit: false,
                fel_intelligence: false,
                mark_of_the_wild: false,
                imp_mark_of_the_wild: false,
                totem_of_wrath: false,
                flametongue: false,
                demonic_pact: false,
                demonic_pact_bonus: 0,
                mana_spring: false,
                restorative_totems: false,
                blessing_of_wisdom: false,
                imp_blessing_of_wisdom: false,
                blessing_of_kings: false,
                drums_of_forgotten_kings: false,
                buff_dmg: false,
                buff_spell_crit: false,
                buff_haste: false,
                buff_spell_haste: false,
                mana_replenishment: false,
                focus_magic: false,
                heroic_presence: false,
                prof_skinning: false,
                prof_alchemy: false,
                prof_engineer: false,

                // Consumes
                food: 0,
                flask: 0,
                battle_elixir: 0,
                guardian_elixir: 0,

                black_magic: false,
                lightweave_embroidery: false,
                darkglow_embroidery: false,
                hyperspeed_accelerators: false,
                ashen_band: false,
                t3_2set: false,
                t5_2set: false,
                t5_4set: false,
                t6_2set: false,
                t6_4set: false,
                t7_2set: false,
                t7_4set: false,
                t8_2set: false,
                t8_4set: false,
                t9_2set: false,
                t9_4set: false,
                t10_2set: false,
                t10_4set: false,
                udc_2set: false,
                udc_3set: false,
                udc_4set: false,
                cudc_3set: false,

                pre_mirror_image: false,
                pre_water_elemental: false,
                pre_incanters_absorption: false,
                pre_mana_incanters_absorption: false,

                wrist_socket: false,
                hands_socket: false,
                waist_socket: false,

                potion: constants.potions.POTION_MANA,
                pre_potion: constants.potions.POTION_SPEED,
                conjured: constants.conjureds.CONJURED_NONE,

                meta_gem: 0,
                trinket1: 0,
                trinket2: 0,

                rotation: constants.rotations.ROTATION_ST_AB_AM,
                rot_ab3_mana: 0,
                rot_ab_no_mb_mana: 100,
                rot_abs_ap: 0,
                rot_mb_below_ab: 0,
                rot_mb_mana: 0,
                rot_ice_lance: false,
                rot_brain_freeze_fireball: false,
                rot_brain_freeze_hold: 15,
                rot_black_magic: false,
                rot_black_magic_ench: 0,
                ignite_munching: false,
                hot_streak_cqs: false,
                hot_streak_cqs_time: 100,
                evo_ticks: 0,

                timings: Array(),
                interruptions: Array(),

                build: "",

                stats: {
                    intellect: 0,
                    spirit: 0,
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    haste: 0,
                    spell_power: 0,
                    crit_rating: 0,
                    hit_rating: 0,
                    haste_rating: 0,
                },

                custom_stats: {
                    intellect: 0,
                    spirit: 0,
                    mp5: 0,
                    spell_power: 0,
                    crit_rating: 0,
                    hit_rating: 0,
                    haste_rating: 0,
                },

                talents: {
                    arcane_focus: 0,
                    clearcast: 0,
                    magic_attunement: 0,
                    spell_impact: 0,
                    student_of_the_mind: 0,
                    focus_magic: 0,
                    arcane_shielding: 0,
                    arcane_meditation: 0,
                    torment_of_the_weak: 0,
                    presence_of_mind: 0,
                    arcane_mind: 0,
                    arcane_instability: 0,
                    arcane_potency: 0,
                    arcane_empowerment: 0,
                    arcane_power: 0,
                    incanters_absorption: 0,
                    arcane_floes: 0,
                    mind_mastery: 0,
                    missile_barrage: 0,
                    netherwind_presence: 0,
                    spell_power: 0,
                    arcane_barrage: 0,

                    imp_fire_blast: 0,
                    incineration: 0,
                    imp_fireball: 0,
                    ignite: 0,
                    world_in_flames: 0,
                    pyroblast: 0,
                    imp_scorch: 0,
                    master_of_elements: 0,
                    playing_with_fire: 0,
                    critical_mass: 0,
                    blast_weave: 0,
                    fire_power: 0,
                    pyromaniac: 0,
                    combustion: 0,
                    molten_fury: 0,
                    empowered_fire: 0,
                    firestarter: 0,
                    dragons_breath: 0,
                    hot_streak: 0,
                    burnout: 0,
                    living_bomb: 0,

                    imp_frostbolt: 0,
                    ice_floes: 0,
                    ice_shards: 0,
                    precision: 0,
                    piercing_ice: 0,
                    icy_veins: 0,
                    frost_channeling: 0,
                    shatter: 0,
                    cold_snap: 0,
                    imp_cone_of_cold: 0,
                    cold_as_ice: 0,
                    winters_chill: 0,
                    arctic_winds: 0,
                    empowered_frostbolt: 0,
                    fingers_of_frost: 0,
                    brain_freeze: 0,
                    water_elemental: 0,
                    enduring_winter: 0,
                    chilled_to_the_bone: 0,
                    deep_freeze: 0,
                },

                glyphs: {
                    arcane_barrage: false,
                    arcane_blast: false,
                    arcane_explosion: false,
                    arcane_missiles: false,
                    arcane_power: false,
                    eternal_water: false,
                    fireball: false,
                    frostbolt: false,
                    frostfire: false,
                    ice_lance: false,
                    living_bomb: false,
                    mage_armor: false,
                    mana_gem: false,
                    mirror_image: false,
                    molten_armor: false,
                    scorch: false,
                    water_elemental: false,
                    blast_wave: false,
                },

                tooltips: false,
            };

            var data = {
                ...constants,
                donation_open: false,
                quick_start_open: false,
                items: items,
                equipped: {},
                enchants: {},
                gems: {},
                item_gems: {},
                item_comparison: [],
                item_off_hand: null,
                item_sort: {
                    name: null,
                    order: null,
                },
                default_profiles: [],
                profiles: [],
                history: [],
                favorites: [],
                active_slot: "weapon",
                new_profile: null,
                import_profile: {
                    open: false,
                    string: null,
                    items: true,
                    config: true,
                },
                export_profile: {
                    open: false,
                    string: null,
                    items: true,
                    config: true,
                },
                import_status: {
                    items: true,
                    config: true,
                },
                profile_status: {
                    open: false,
                    timeout: null,
                    items: true,
                    missing_items: [],
                    config: true,
                },
                error_notice: {
                    open: false,
                    timeout: null,
                    title: null,
                    text: [],
                },
                custom_item: {
                    id: null,
                    title: null,
                    slot: null,
                    q: "epic",
                    sockets: null,
                    meta_socket: false,
                    int: null,
                    spi: null,
                    sp: null,
                    crit: null,
                    hit: null,
                    haste: null,
                    mp5: null,
                    twohand: false,
                },
                custom_item_open: false,
                custom_item_error: null,
                custom_stats_open: false,
                equiplist_open: false,
                equiplist_string: null,
                display_stats: null,
                result: null,
                pin_dps: null,
                ep_result: null,
                ep_weight: "dps",
                is_running: false,
                is_running_ep: false,
                active_tab: "gear",
                item_source: "wowhead",
                search_item: "phase:2- ",
                search_gem: "",
                search_log: "",
                log_filter: {
                    "0": true,
                    "1": false,
                    "2": false,
                    "3": true,
                    "4": false,
                    "5": true,
                    "6": true,
                    "7": true,
                    "8": false,
                    "9": true,
                    "10": true,
                },
                log_filter_player: false,
                splitview: splitview,
                talent_map: [[],[],[]],
                default_config: default_config,
                config: _.cloneDeep(default_config),
            };

            var slots = [
                "weapon", "off_hand", "ranged",
                "head", "neck", "shoulder", "back", "chest", "wrist",
                "hands", "waist", "legs", "feet",
                "finger1", "finger2", "trinket1", "trinket2",
            ];

            for (var slot of slots) {
                var islot = slot;
                var i = 0;
                if (slot.indexOf("finger") === 0 || slot.indexOf("trinket") === 0) {
                    islot = slot.substr(0, slot.length-1);
                    i = parseInt(slot.substr(slot.length-1))-1;
                }
                data.equipped[slot] = null;
                data.enchants[slot] = null;
                data.gems[slot] = [null, null, null];
            }

            data.slots = [...slots];

            data.talent_map[0][1] = "arcane_focus";
            data.talent_map[0][5] = "clearcast";
            data.talent_map[0][6] = "magic_attunement";
            data.talent_map[0][7] = "spell_impact";
            data.talent_map[0][8] = "student_of_the_mind";
            data.talent_map[0][9] = "focus_magic";
            data.talent_map[0][10] = "arcane_shielding";
            data.talent_map[0][12] = "arcane_meditation";
            data.talent_map[0][13] = "torment_of_the_weak";
            data.talent_map[0][15] = "presence_of_mind";
            data.talent_map[0][16] = "arcane_mind";
            data.talent_map[0][18] = "arcane_instability";
            data.talent_map[0][19] = "arcane_potency";
            data.talent_map[0][20] = "arcane_empowerment";
            data.talent_map[0][21] = "arcane_power";
            data.talent_map[0][22] = "incanters_absorption";
            data.talent_map[0][23] = "arcane_floes";
            data.talent_map[0][24] = "mind_mastery";
            data.talent_map[0][26] = "missile_barrage";
            data.talent_map[0][27] = "netherwind_presence";
            data.talent_map[0][28] = "spell_power";
            data.talent_map[0][29] = "arcane_barrage";

            data.talent_map[1][0] = "imp_fire_blast";
            data.talent_map[1][1] = "incineration";
            data.talent_map[1][2] = "imp_fireball";
            data.talent_map[1][3] = "ignite";
            data.talent_map[1][5] = "world_in_flames";
            data.talent_map[1][8] = "pyroblast";
            data.talent_map[1][10] = "imp_scorch";
            data.talent_map[1][12] = "master_of_elements";
            data.talent_map[1][13] = "playing_with_fire";
            data.talent_map[1][14] = "critical_mass";
            data.talent_map[1][15] = "blast_weave";
            data.talent_map[1][17] = "fire_power";
            data.talent_map[1][18] = "pyromaniac";
            data.talent_map[1][19] = "combustion";
            data.talent_map[1][20] = "molten_fury";
            data.talent_map[1][22] = "empowered_fire";
            data.talent_map[1][23] = "firestarter";
            data.talent_map[1][24] = "dragons_breath";
            data.talent_map[1][25] = "hot_streak";
            data.talent_map[1][26] = "burnout";
            data.talent_map[1][27] = "living_bomb";

            data.talent_map[2][1] = "imp_frostbolt";
            data.talent_map[2][2] = "ice_floes";
            data.talent_map[2][3] = "ice_shards";
            data.talent_map[2][5] = "precision";
            data.talent_map[2][7] = "piercing_ice";
            data.talent_map[2][8] = "icy_veins";
            data.talent_map[2][11] = "frost_channeling";
            data.talent_map[2][12] = "shatter";
            data.talent_map[2][13] = "cold_snap";
            data.talent_map[2][14] = "imp_cone_of_cold";
            data.talent_map[2][16] = "cold_as_ice";
            data.talent_map[2][17] = "winters_chill";
            data.talent_map[2][20] = "arctic_winds";
            data.talent_map[2][21] = "empowered_frostbolt";
            data.talent_map[2][22] = "fingers_of_frost";
            data.talent_map[2][23] = "brain_freeze";
            data.talent_map[2][24] = "water_elemental";
            data.talent_map[2][25] = "enduring_winter";
            data.talent_map[2][26] = "chilled_to_the_bone";
            data.talent_map[2][27] = "deep_freeze";

            return data;
        },

        computed: {
            faction() {
                var alliance = [
                    this.races.RACE_GNOME,
                    this.races.RACE_HUMAN,
                    this.races.RACE_DRAENEI,
                ];

                if (alliance.indexOf(this.config.race) != -1)
                    return "alliance";
                return "horde";
            },

            spellStats() {
                if (!this.result.spells)
                    return [];
                return this.result.spells.sort(function(a, b) {
                    if (a.source == "Player" && b.source != "Player")
                        return -1;
                    if (b.source == "Player" && a.source != "Player")
                        return 1;
                    return a.name.localeCompare(b.name);
                });
            },

            canBlackMagicWeave() {
                if (this.enchants.weapon == this.items.ids.BLACK_MAGIC)
                    return false;
                if (!this.equipped.weapon)
                    return false;
                var weapon = this.getItem("weapon", this.equipped.weapon);
                if (_.get(weapon, "unique"))
                    return false;
                return true;
            },

            activeItems() {
                var slot = this.equipSlotToItemSlot(this.active_slot);

                var items = this.items.equip[slot];

                if (!items)
                    return [];

                if (this.search_item) {
                    var terms = this.search_item.toLowerCase().split(" ");
                    var str = [];
                    var last_str = false;
                    var arr, value, op;

                    var fnBool = function(str) {
                        if (str[0] == "y")
                            return true;
                        return false;
                    };

                    var filterBool = function(str, val) {
                        if (str[0] == "y")
                            return val;
                        return !val;
                    };

                    var filterInt = function(str, val) {
                        var m;
                        if (m = str.match(/^([0-9]+)\-([0-9]+)$/)) {
                            return val >= m[1] && val <= m[2];
                        }
                        else if (m = str.match(/^([0-9]+)$/)) {
                            return val == m[1];
                        }
                        else if (m = str.match(/^([0-9]+)\+$/)) {
                            return val >= m[1];
                        }
                        else if (m = str.match(/^([0-9]+)\-$/)) {
                            return val <= m[1];
                        }

                        return true;
                    };

                    for (var i=0; i<terms.length; i++) {
                        arr = terms[i].split(":");
                        if (arr.length == 2) {
                            if (!arr[1].length)
                                continue;
                            if (arr[0] == "pvp") {
                                value = fnBool(arr[1]);
                                if (value)
                                    items = items.filter(item => item.title.indexOf(" Gladiator's ") != -1);
                                else if (value !== null)
                                    items = items.filter(item => item.title.indexOf(" Gladiator's ") == -1);
                            }
                            else if (arr[0] == "ilvl") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "ilvl", 1)));
                            }
                            else if (arr[0] == "p" || arr[0] == "phase") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "phase", 1)));
                            }
                            else if (arr[0] == "sp") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "sp", 1)));
                            }
                            else if (arr[0] == "crit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "crit", 1)));
                            }
                            else if (arr[0] == "hit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "hit", 1)));
                            }
                            else if (arr[0] == "haste") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "haste", 1)));
                            }
                            else if (arr[0] == "spi" || arr[0] == "spirit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "spi", 1)));
                            }
                            else if (arr[0] == "int" || arr[0] == "intellect") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "int", 1)));
                            }
                            else if (arr[0] == "mp5") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "mp5", 1)));
                            }
                            else if (arr[0] == "2h") {
                                items = items.filter(item => filterBool(arr[1], _.get(item, "twohand", false)));
                            }
                            else if (arr[0] == "set" || arr[0] == "itemset") {
                                items = items.filter(item => filterBool(arr[1], _.get(item, "itemset", false)));
                            }

                            last_str = false;
                        }
                        else {
                            if (last_str)
                                str[str.length-1]+= " "+terms[i];
                            else
                                str.push(terms[i]);
                            last_str = true;
                        }
                    }

                    if (str.length) {
                        for (var i=0; i<str.length; i++)
                            items = items.filter(item => item.title.toLowerCase().indexOf(str[i]) != -1);
                    }
                }

                return this.sort(items, this.item_sort);
            },

            activeEnchants() {
                var slot = this.equipSlotToEnchantSlot(this.active_slot);

                if (!this.items.enchants.hasOwnProperty(slot))
                    return [];

                return this.items.enchants[slot];
            },

            activeSockets() {
                return this.slotSockets(this.active_slot);
            },

            activeLog() {
                if (!this.result || !this.result.log)
                    return [];

                var log = this.result.log;

                if (this.log_filter_player)
                    log = log.filter(l => l.unit == "Player");

                if (this.search_log)
                    log = log.filter(l => l.text.toLowerCase().indexOf(this.search_log.toLowerCase()) != -1);

                log = log.filter(l => this.log_filter[l.type]);

                return log;
            },

            hasComparisons() {
                return this.item_comparison.length > 1;
            },

            itemSlots() {
                return _.keys(this.items.equip);
            },

            numCasts() {
                if (!this.result || !this.result.spells)
                    return 0;
                return _.sumBy(this.result.spells, "casts");
            },

            epCalc() {
                if (!this.ep_result)
                    return null;

                var ep = {
                    intellect: null,
                    spirit: null,
                    mp5: null,
                    spell_power: null,
                    crit_rating: null,
                    hit_rating: null,
                    haste_rating: null,
                };

                if (!this.ep_result.base)
                    return ep;

                if (this.ep_weight == "dps") {
                    for (var stat in ep) {
                        if (this.ep_result[stat])
                            ep[stat] = (this.ep_result[stat] - this.ep_result.base) / 10;
                    }
                }
                else {
                   if (_.get(this.ep_result, this.ep_weight, 0) - this.ep_result.base < 0.1)
                        return ep;

                    for (var stat in ep) {
                        if (this.ep_result[stat]) {
                            if (stat == this.ep_weight)
                                ep[stat] = 1;
                            else
                                ep[stat] = (this.ep_result[stat] - this.ep_result.base) / (this.ep_result[this.ep_weight] - this.ep_result.base);
                        }
                    }
                }

                return ep;
            },

            timings() {
                var timings = [];

                timings.push({
                    name: "bloodlust",
                    title: "Bloodlust",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_bloodlust.jpg",
                });
                timings.push({
                    name: "power_infusion",
                    title: "Power Infusion",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_holy_powerinfusion.jpg",
                });
                timings.push({
                    name: "mana_tide",
                    title: "Mana Tide",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_summonwaterelemental.jpg",
                });
                timings.push({
                    name: "innervate",
                    title: "Innervate",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "mana_gem",
                    title: "Mana Gem",
                    icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_gem_sapphire_02.jpg",
                });
                timings.push({
                    name: "evocation",
                    title: "Evocation",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_purge.jpg",
                });
                timings.push({
                    name: "mirror_image",
                    title: "Mirror Image",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_magic_lesserinvisibilty.jpg",
                });
                timings.push({
                    name: "water_elemental",
                    title: "Water Elemental",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_summonwaterelemental_2.jpg",
                });
                timings.push({
                    name: "berserking",
                    title: "Berserking",
                    icon: "https://www.wowhead.com/images/wow/icons/large/racial_troll_berserk.jpg",
                });
                timings.push({
                    name: "presence_of_mind",
                    title: "Presence of Mind",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_enchantarmor.jpg",
                });
                timings.push({
                    name: "arcane_power",
                    title: "Arcane Power",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "combustion",
                    title: "Combustion",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_fire_sealoffire.jpg",
                });
                timings.push({
                    name: "icy_veins",
                    title: "Icy Veins",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_coldhearted.jpg",
                });
                timings.push({
                    name: "cold_snap",
                    title: "Cold Snap",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_wizardmark.jpg",
                });
                timings.push({
                    name: "hyperspeed_accelerators",
                    title: "Hyperspeed Accelerators",
                    icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_enggizmos_04.jpg",
                });
                timings.push({
                    name: "sapper_charge",
                    title: "Sapper Charge",
                    icon: "https://www.wowhead.com/images/wow/icons/large/inv_gizmo_supersappercharge.jpg",
                });

                var potion = {
                    name: "potion",
                    title: "Runic Mana Potion",
                    icon: "https://www.wowhead.com/images/wow/icons/large/inv_alchemy_elixir_02.jpg",
                };
                if (this.config.potion == constants.potions.POTION_SPEED) {
                    potion.title = "Potion of Speed";
                    potion.icon = "https://www.wowhead.com/images/wow/icons/large/inv_alchemy_elixir_04.jpg";
                }
                else if (this.config.potion == constants.potions.POTION_WILD_MAGIC) {
                    potion.title = "Potion of Wild Magic";
                    potion.icon = "https://www.wowhead.com/images/wow/icons/large/inv_alchemy_elixir_01.jpg";
                }
                else if (this.config.potion == constants.potions.POTION_FLAME_CAP) {
                    potion.title = "Flame Cap";
                    potion.icon = "https://www.wowhead.com/images/wow/icons/large/inv_misc_herb_flamecap.jpg";
                }
                timings.push(potion);

                if (this.config.conjured == constants.conjureds.CONJURED_DARK_RUNE) {
                    timings.push({
                        name: "conjured",
                        title: "Dark Rune",
                        icon: "https://wow.zamimg.com/images/wow/icons/large/spell_shadow_sealofkings.jpg",
                    });
                }

                var trinkets = [
                    {
                        id: this.items.ids.TRINKET_SLIVER_PURE_ICE_HC,
                        title: "Sliver of Pure Ice",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_trinket_05.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SLIVER_PURE_ICE_NM,
                        title: "Sliver of Pure Ice",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_trinket_05.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_VOLATILE_POWER_HC,
                        title: "Talisman of Volatile Power",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_necklace_27.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_VOLATILE_POWER_NM,
                        title: "Talisman of Volatile Power",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_necklace_27.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SCALE_FATES,
                        title: "Scale of Fates",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_spiritshard_02.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_LIVING_FLAME,
                        title: "Living Flame",
                        icon: "https://www.wowhead.com/images/wow/icons/large/spell_fire_burnout.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_TWILIGHT_SERPENT,
                        title: "Figurine - Twilight Serpent",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelcrafting_purpleserpent.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_TOME_ARCANE_PHENOMENA,
                        title: "Tome of Arcane Phenomena",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_book_07.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_CANNONEERS_FUSELIGHTER,
                        title: "Cannoneer's Fuselighter",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_gizmo_elementalblastingpowder.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_MARK_WAR_PRISONER,
                        title: "Mark of the War Prisoner",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_talisman_13.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_RUNE_INFINITE_POWER,
                        title: "Rune of Infinite Power",
                        icon: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_rune_10.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_NAARU_SLIVER,
                        title: "Shifting Naaru Sliver",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_jewelry_talisman_15.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SKULL_GULDAN,
                        title: "Skull of Gul'dan",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_bone_elfskull_01.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SHRUNKEN_HEAD,
                        title: "Hex Shrunken Head",
                        icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_head_troll_01.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_MQG,
                        title: "Mind Quickening Gem",
                        icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_wispheal.jpg",
                    },
                ];

                var slots = ["trinket1", "trinket2"];
                var timing;
                for (var i=0; i<trinkets.length; i++) {
                    for (var j=0; j<slots.length; j++) {
                        if (this.equipped[slots[j]] == trinkets[i].id) {
                            timing = _.clone(trinkets[i]);
                            delete timing.id;
                            timing.name = slots[j];
                            timings.push(timing);
                            slots.splice(j, 1);
                            j--;
                        }
                    }
                }

                // Encounters
                timings.push({
                    name: "hodir_storm_power",
                    title: "Hodir: Storm Power",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_shaman_staticshock.jpg",
                });
                timings.push({
                    name: "hodir_starlight",
                    title: "Hodir: Starlight (50 sec)",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_elunesgrace.jpg",
                });
                timings.push({
                    name: "hodir_singed",
                    title: "Hodir: Singed (25 stacks)",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_summerfest_brazierorange.jpg",
                });
                timings.push({
                    name: "iron_council_shield_of_runes",
                    title: "Iron Council: Shield of Runes (popped)",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_deathknight_runetap.jpg",
                });

                return timings;
            },

            waitBuffs() {
                var buffs = [];

                if (this.config.talents.missile_barrage)
                    buffs.push({id: constants.buffs.MISSILE_BARRAGE, name: "Missile Barrage"});
                if (this.config.talents.fingers_of_frost)
                    buffs.push({id: constants.buffs.FINGERS_OF_FROST, name: "Fingers of Frost"});
                if (this.config.talents.brain_freeze)
                    buffs.push({id: constants.buffs.BRAIN_FREEZE, name: "Brain Freeze"});
                if (this.config.talents.hot_streak)
                    buffs.push({id: constants.buffs.HOT_STREAK, name: "Hot Streak"});
                if (this.config.talents.firestarter)
                    buffs.push({id: constants.buffs.FIRESTARTER, name: "Firestarter"});
                if (this.config.talents.molten_fury)
                    buffs.push({id: constants.buffs.MOLTEN_FURY, name: "Molten Fury"});

                if (this.enchants.weapon == this.items.ids.BLACK_MAGIC || this.config.rot_black_magic)
                    buffs.push({id: constants.buffs.BLACK_MAGIC, name: "Black Magic"});
                if (this.enchants.back == this.items.ids.LIGHTWEAVE_EMBROIDERY)
                    buffs.push({id: constants.buffs.LIGHTWEAVE, name: "Lightweave Embroidery"});

                if (this.numEquippedSet(this.items.ids.T8_SET) > 1)
                    buffs.push({id: constants.buffs.PRAXIS, name: "Praxis (t8 2set)"});
                if (this.numEquippedSet(this.items.ids.T10_SET) > 1)
                    buffs.push({id: constants.buffs.PUSHING_THE_LIMIT, name: "Pushing the Limit (t10 2set)"});

                if (this.isEquipped("trinket", this.items.ids.TRINKET_FORGE_EMBER))
                    buffs.push({id: constants.buffs.FORGE_EMBER, name: "Forge Ember"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_SUNDIAL_EXILED))
                    buffs.push({id: constants.buffs.NOW_IS_THE_TIME, name: "Sundial of the Exiled"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_FLARE_HEAVENS))
                    buffs.push({id: constants.buffs.FLARE_HEAVENS, name: "Flare of the Heavens"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_PANDORAS_PLEA))
                    buffs.push({id: constants.buffs.PANDORAS_PLEA, name: "Pandora's Plea"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_EMBRACE_SPIDER))
                    buffs.push({id: constants.buffs.EMBRACE_SPIDER, name: "Embrace of the Spider"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_DYING_CURSE))
                    buffs.push({id: constants.buffs.DYING_CURSE, name: "Dying Curse"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_ELEMENTAL_FOCUS_STONE))
                    buffs.push({id: constants.buffs.ALACRITY_ELEMENTS, name: "Elemental Focus Stone"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_DISLODGED_OBJECT_HC))
                    buffs.push({id: constants.buffs.DISLODGED_OBJECT_HC, name: "Dislodged Foreign Object"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_DISLODGED_OBJECT_NM))
                    buffs.push({id: constants.buffs.DISLODGED_OBJECT_NM, name: "Dislodged Foreign Object"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_NAMELESS_LICH_HC))
                    buffs.push({id: constants.buffs.NAMELESS_LICH_HC, name: "Phylactery of the Nameless Lich"});
                if (this.isEquipped("trinket", this.items.ids.TRINKET_NAMELESS_LICH_NM))
                    buffs.push({id: constants.buffs.NAMELESS_LICH_NM, name: "Phylactery of the Nameless Lich"});

                if (this.isEquipped("finger", this.items.ids.ASHEN_BAND_ENDLESS_DESTRUCTION) || this.isEquipped("finger", this.items.ids.ASHEN_BAND_UNMATCHED_DESTRUCTION))
                    buffs.push({id: constants.buffs.ASHEN_BAND, name: "Frostforged Sage (Ashen Band)"});
                else if (this.isEquipped("finger", this.items.ids.ASHEN_BAND_ENDLESS_WISDOM) || this.isEquipped("finger", this.items.ids.ASHEN_BAND_UNMATCHED_WISDOM))
                    buffs.push({id: constants.buffs.ASHEN_BAND, name: "Frostforged Sage (Ashen Band)"});

                buffs = _.sortBy(buffs, "name");

                return buffs;
            }
        },

        methods: {
            setDesign(design) {
                document.documentElement.className = document.documentElement.className.replace(/design-[0-9]+/);
                localStorage.setItem("design", design);
                document.documentElement.classList.add("design-"+design);
            },

            onDesignInput(e) {
                this.setDesign(e.target.value);
            },

            setSplitview(value) {
                localStorage.setItem("splitview", value);
                this.splitview = value;
                if (this.splitview)
                    this.refreshTooltips();
            },

            toggleSplitview(e) {
                this.setSplitview(!this.splitview);
            },

            splitviewShort(long, short) {
                return this.splitview ? short : long;
            },

            newTimingId() {
                var id = 0;
                for (var i=0; i<this.config.timings.length; i++)
                    id = Math.max(id, this.config.timings[i].id);
                return id+1;
            },

            addTiming(name) {
                this.config.timings.push({
                    id: this.newTimingId(),
                    name: name,
                    t: 0,
                    wait_t: 0,
                    wait_for_buff: 0,
                });

                this.config.timings = _.sortBy(this.config.timings, "name");
            },

            removeTiming(id) {
                var index = _.findIndex(this.config.timings, {id: id});
                if (index != -1)
                    this.config.timings.splice(index, 1);
            },

            getTiming(name, key, def) {
                if (typeof(def) == "undefined")
                    def = null;
                var timing = _.find(this.timings, {name: name});
                if (!timing)
                    return def;
                return _.get(timing, key, def);
            },

            timingEnabled(name) {
                var always = [
                    "bloodlust", "mana_tide", "power_infusion",
                    "innervate", "mana_gem", "evocation", "mirror_image",
                    "dark_rune",
                ];
                if (always.indexOf(name) != -1)
                    return true;

                if (name == "berserking")
                    return this.config.race == constants.races.RACE_TROLL;
                if (name == "potion")
                    return this.config.potion != constants.potions.POTION_NONE;
                if (name == "conjured")
                    return this.config.conjured != constants.conjureds.CONJURED_NONE;
                if (name == "hyperspeed_accelerators")
                    return this.enchants.hands == this.items.ids.HYPERSPEED_ACCELERATORS;
                if (name == "sapper_charge")
                    return this.config.talents.incanters_absorption && this.config.pre_incanters_absorption;
                if (name == "presence_of_mind")
                    return this.config.talents.presence_of_mind > 0;
                if (name == "arcane_power")
                    return this.config.talents.arcane_power > 0;
                if (name == "combustion")
                    return this.config.talents.combustion > 0;
                if (name == "icy_veins")
                    return this.config.talents.icy_veins > 0;
                if (name == "cold_snap")
                    return this.config.talents.cold_snap > 0;
                if (name == "water_elemental")
                    return this.config.talents.water_elemental > 0;
                if (name == "trinket1")
                    return this.equipped.trinket1 && _.get(this.equippedItem("trinket1"), "use");
                if (name == "trinket2")
                    return this.equipped.trinket2 && _.get(this.equippedItem("trinket2"), "use");

                if (this.config.encounters) {
                    var encounters = [
                        "hodir_storm_power", "hodir_starlight", "hodir_singed",
                        "iron_council_shield_of_runes",
                    ];
                    if (encounters.indexOf(name) != -1)
                        return true;
                }

                return false;
            },

            timingCanWait(name) {
                var nowait = ["bloodlust", "mana_tide", "power_infusion", "innervate"];
                if (nowait.indexOf(name) != -1)
                    return false;

                return true;
            },

            newInterruptionId() {
                var id = 0;
                for (var i=0; i<this.config.interruptions.length; i++)
                    id = Math.max(id, this.config.interruptions[i].id);
                return id+1;
            },

            addInterruption() {
                this.config.interruptions.push({
                    id: this.newInterruptionId(),
                    silence: false,
                    affects_all: false,
                    t: 0,
                    duration: 0,
                });

                this.config.interruptions = _.sortBy(this.config.interruptions, "t");
            },

            removeInterruption(id) {
                var index = _.findIndex(this.config.interruptions, {id: id});
                if (index != -1)
                    this.config.interruptions.splice(index, 1);
            },

            sort(items, sorting) {
                var self = this;

                if (!sorting || !sorting.name) {
                    return items.sort(function(a, b) {
                        var fa = self.isFavorite(a);
                        var fb = self.isFavorite(b);
                        if (fa && !fb)
                            return -1;
                        if (!fa && fb)
                            return 1;
                        return 0;
                    });
                }

                var type = null;
                for (var i=0; i<items.length; i++) {
                    var value = _.get(items[i], sorting.name, null);
                    if (value !== null) {
                        type = typeof(value);
                        if (type == "object") {
                            if (_.isArray(value))
                                type = "array";
                            else
                                continue;
                        }
                        break;
                    }
                }

                if (sorting.name == "dps")
                    type = "number";

                if (type === null)
                    return items;

                return items.sort(function(a, b) {
                    var av = _.get(a, sorting.name, null);
                    var bv = _.get(b, sorting.name, null);

                    if (sorting.name == "phase") {
                        if (!av) av = 1;
                        if (!bv) bv = 1;
                    }

                    if (sorting.name == "dps") {
                        av = _.get(_.find(self.item_comparison, {id: a.id}), "dps", 0);
                        bv = _.get(_.find(self.item_comparison, {id: b.id}), "dps", 0);
                    }

                    var result = 0;
                    if (type == "string") {
                        try { av = av.toString(); } catch(e) { av = ""; };
                        try { bv = bv.toString(); } catch(e) { bv = ""; };
                        result = av.localeCompare(bv);
                    }
                    else if (type == "number") {
                        av = parseFloat(av);
                        bv = parseFloat(bv);
                        if (isNaN(av)) av = 0;
                        if (isNaN(bv)) bv = 0;
                        result = av - bv;
                    }
                    else if (type == "array") {
                        av = _.get(av, "length", 0);
                        bv = _.get(bv, "length", 0);
                        result = av - bv;
                    }

                    if (sorting.order == "desc" && result != 0)
                        result = result < 0 ? 1 : -1;

                    return result;
                });
            },

            checkDonation() {
                if (window.location.hash == "#donation") {
                    window.location.hash = "";
                    this.donation_open = true;
                }
            },

            checkNewUser() {
                var storages = ["magesim_wotlk_profile", "magesim_wotlk_profiles", "magesim_wotlk_custom_items"];
                for (var key of storages) {
                    if (localStorage.getItem(key) !== null)
                        return;
                }

                this.quick_start_open = true;
            },

            runMultiple() {
                var self = this;

                if (["log", "timeline", "spells"].indexOf(this.active_tab) != -1)
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                var save_history = this.saveHistory();
                this.is_running = true;

                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                    if (save_history) {
                        self.history[0].result = result;
                        self.history[0].end = new Date;
                    }
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                sim.start(this.config);
            },

            runSingle() {
                var self = this;
                var sim = new SimulationWorker((result) => {
                    self.is_running = false;
                    self.result = result;
                    if (self.result.spells)
                        self.result.spells = _.sortBy(self.result.spells, "casts").reverse();
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                if (this.active_tab == "histogram")
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                this.is_running = true;
                sim.start(this.config);
            },

            async findAvg(avg) {
                if (this.active_tab == "histogram")
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                this.is_running = true;

                var result;
                while (true) {
                    result = await this.runAvg(avg);
                    if (Math.abs(result.dps - avg) <= avg*0.005) {
                        this.is_running = false;
                        this.result = result;
                        break;
                    }
                }
            },

            async runAvg(avg) {
                var self = this;

                return new Promise(function(resolve, reject) {
                    var sim = new SimulationWorker((result) => {
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                    });

                    self.is_running = true;
                    sim.start(self.config);
                });
            },

            async runStat(stat, value, rng_seed) {
                var self = this;

                return new Promise((resolve, reject) => {
                    var sim = new SimulationWorkers(self.config.iterations, (result) => {
                        self.is_running = false;
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                        reject(error);
                    });

                    if (self.config.custom_stats[stat] == "")
                        self.config.custom_stats[stat] = 0;
                    self.config.custom_stats[stat]+= value;
                    self.prepare();
                    var config = _.cloneDeep(self.config);
                    self.config.custom_stats[stat]-= value;
                    if (rng_seed)
                        config.rng_seed = rng_seed;
                    self.is_running = true;
                    sim.start(config);
                });
            },

            async runEP() {
                if (this.is_running_ep)
                    this.is_running_ep = false;

                if (this.is_running)
                    return;

                if (["log", "timeline", "spells"].indexOf(this.active_tab) != -1)
                    this.setTab("gear");

                this.is_running_ep = true;
                this.result = null;
                this.ep_result = {
                    base: null,
                    intellect: null,
                    spirit: null,
                    mp5: null,
                    spell_power: null,
                    crit_rating: null,
                    hit_rating: null,
                    haste_rating: null,
                };

                var rng_seed = Math.round(Math.random() * 100000);
                var result;
                var increment = this.config.stat_weight_increment;
                if (increment < 1)
                    increment = 20;
                for (var stat in this.ep_result) {
                    result = await this.runStat(stat, stat == "base" ? 0 : increment, rng_seed);
                    this.ep_result[stat] = result.avg_dps;
                    if (!this.is_running_ep)
                        break;
                }

                this.is_running_ep = false;
            },

            async runComparisonFor(item_id) {
                var self = this;
                this.equip(this.active_slot, item_id, false);

                return new Promise((resolve, reject) => {
                    var sim = new SimulationWorkers(self.config.iterations, (result) => {
                        self.is_running = false;
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                        reject(error);
                    });

                    self.prepare();
                    self.is_running = true;
                    sim.start(self.config);
                });
            },

            async runComparison() {
                if (!this.hasComparisons || this.is_running)
                    return;

                for (var i in this.item_comparison)
                    this.item_comparison[i].dps = null;

                var old_item_id = this.equipped[this.active_slot];
                var result, cmp;
                for (var i in this.item_comparison) {
                    cmp = this.item_comparison[i];
                    result = await this.runComparisonFor(cmp.id);
                    this.item_comparison[i].dps = result.avg_dps;
                    this.item_comparison[i].is_meta_gem_active = this.isMetaGemActive();
                }

                this.equip(this.active_slot, old_item_id);
            },

            prepare() {
                this.fillEmptyFields();
                this.sortTimings();
                this.saveCurrentProfile();
                this.itemStats();
                this.itemConfig();
                this.calcStats();
            },

            clearSearchText() {
                var str = [];
                var terms = this.search_item.toLowerCase().split(" ");

                for (var i=0; i<terms.length; i++) {
                    if (terms[i].split(":").length == 2)
                        str.push(terms[i]);
                }

                this.search_item = str.join(" ")+" ";
            },

            setActiveSlot(slot) {
                if (this.is_running)
                    return;

                this.active_slot = slot;
                this.item_comparison = [];

                this.$nextTick(function() {
                    this.clearSearchText();
                    this.$refs.search.focus();
                    this.refreshTooltips();
                });
            },

            equipSlotToItemSlot(slot) {
                if (slot.indexOf("finger") === 0)
                    slot = "finger";
                if (slot.indexOf("trinket") === 0)
                    slot = "trinket";

                return slot;
            },

            equipSlotToEnchantSlot(slot) {
                return this.equipSlotToItemSlot(slot);
            },

            otherSlot(slot) {
                var n = slot.substr(-1);
                n = parseInt(n);
                if (isNaN(n))
                    return slot;
                n = n == 1 ? 2 : 1;
                return slot.substr(0, slot.length-1)+n;
            },

            dontStack(input, config) {
                if (!Array.isArray(config))
                    config = [config];

                if (input.target.checked) {
                    for (var i=0; i<config.length; i++) {
                        var key = config[i];
                        if (_.get(this.config, key)) {
                            _.set(this.config, key, false);
                        }
                    }
                }
            },

            findItem(id) {
                var item;
                for (var i=0; i<this.itemSlots.length; i++) {
                    item =_.find(this.items.equip[this.itemSlots[i]], {id: id});
                    if (item)
                        return item;
                }

                return null;
            },

            getItem(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.equip[eslot], {id: id}, null);
            },

            searchItem(slot, title) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.equip[eslot], {title: title}, null);
            },

            getGem(id) {
                return _.find(this.items.gems, {id: id}, null);
            },

            searchGem(title) {
                return _.find(this.items.gems, {title: title}, null);
            },

            getEnchant(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {id: id}, null);
            },

            getEnchantFromEnchantmentId(slot, id) {
                var map = {
                    3872: 3719,
                    3873: 3721,
                };

                if (map.hasOwnProperty(id))
                    id = map[id];

                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {enchantmentId: id}, null);
            },

            searchEnchant(slot, title) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {title: title}, null);
            },

            equippedItem(slot) {
                var id = this.equipped[slot];
                if (!id)
                    return null;

                return this.getItem(slot, id);
            },

            equippedEnchant(slot) {
                var id = this.enchants[slot];
                if (!id)
                    return null;

                return this.getEnchant(slot, id);
            },

            dollSlots(pos) {
                if (pos == "left") {
                    return [
                        "head", "neck", "shoulder",
                        "back", "chest", "wrist",
                        "weapon", "off_hand", "ranged",
                    ];
                }
                if (pos == "right") {
                    return [
                        "hands", "waist", "legs", "feet",
                        "finger1", "finger2", "trinket1", "trinket2",
                    ];
                }
            },

            slotSockets(slot, item) {
                if (!item)
                    item = this.equippedItem(slot);
                var sockets = [];
                if (item && item.sockets)
                    sockets = _.clone(item.sockets);
                if (this.hasExtraSocket(slot))
                    sockets.push("a");
                return sockets;
            },

            hasExtraSocket(slot) {
                if (slot == "wrist" && this.config.wrist_socket)
                    return true;
                if (slot == "hands" && this.config.hands_socket)
                    return true;
                if (slot == "waist" && this.config.waist_socket)
                    return true;
                return false;
            },

            activeGems(index) {
                if (this.activeSockets.length < index)
                    return [];

                if (this.activeSockets[index] == "m")
                    var gems = this.items.gems.filter(g => g.color == "m");
                else
                    var gems = this.items.gems.filter(g => g.color != "m");

                if (this.search_gem)
                    gems = gems.filter(g => g.title.toLowerCase().indexOf(this.search_gem.toLowerCase()) != -1);

                var self = this;

                return gems.sort(function(a, b) {
                    var fa = self.isFavorite(a);
                    var fb = self.isFavorite(b);
                    if (fa && !fb)
                        return -1;
                    if (!fa && fb)
                        return 1;
                    return 0;
                });
            },

            fillEmptyFields() {
                for (var key in this.config) {
                    if (this.config[key] === "" || this.config[key] === null)
                        this.config[key] = this.default_config.hasOwnProperty(key) ? this.default_config[key] : 0;
                }
            },

            sortTimings() {
                var fn = function(a, b) {
                    if (a.name == b.name)
                        return a.t - b.t;
                    return a.name.localeCompare(b.name);
                };

                this.config.timings = this.config.timings.filter(t => this.timingEnabled(t.name) || t.name.indexOf("trinket") == 0);

                this.config.timings.sort(fn);
            },

            baseStats() {
                // Undead default
                var stats = {
                    intellect: 179,
                    spirit: 179,
                    mp5: 0,
                    crit: 0.91,
                    hit: 0,
                    haste: 0,
                    spell_power: 0,
                    crit_rating: 0,
                    hit_rating: 0,
                    haste_rating: 0,
                };

                if (this.config.race == this.races.RACE_TROLL) {
                    stats.intellect = 177;
                    stats.spirit = 175;
                }
                if (this.config.race == this.races.RACE_BLOOD_ELF) {
                    stats.intellect = 184;
                    stats.spirit = 172;
                }
                if (this.config.race == this.races.RACE_DRAENEI) {
                    stats.intellect = 181;
                    stats.spirit = 176;
                }
                if (this.config.race == this.races.RACE_GNOME) {
                    stats.intellect = 184;
                    stats.spirit = 174;
                }
                if (this.config.race == this.races.RACE_HUMAN) {
                    stats.intellect = 181;
                    stats.spirit = 174;
                }

                return stats;
            },

            itemStats() {
                var stats = this.baseStats();

                var item_stats = {
                    int: 0,
                    spi: 0,
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    haste: 0,
                };

                var addStats = function(itm) {
                    for (var stat in itm) {
                        if (item_stats.hasOwnProperty(stat))
                            item_stats[stat]+= itm[stat];
                    }
                };

                var slot, item, has_bonus, get_bonus, enchant, gem, gem_id, set, setbonus, sockets;
                var sets = {};

                for (var key in this.equipped) {
                    slot = this.equipSlotToItemSlot(key);
                    item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item) {
                        has_bonus = item.hasOwnProperty("bonus");
                        get_bonus = true;

                        addStats(item);

                        if (this.items.enchants[slot] && this.enchants[key]) {
                            enchant = _.find(this.items.enchants[slot], {id: this.enchants[key]});
                            if (enchant)
                                addStats(enchant);
                        }

                        sockets = this.slotSockets(key);
                        if (sockets) {
                            for (var i=0; i<sockets.length; i++) {
                                gem_id = this.gems[key][i];
                                gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                                if (gem && (gem.color != "m" || this.isMetaGemActive()))
                                    addStats(gem);
                            }

                            if (has_bonus && this.hasSocketBonus(key))
                                addStats(item.bonus);
                        }

                        if (item.itemset) {
                            if (!sets.hasOwnProperty(item.itemset)) {
                                sets[item.itemset] = 1;
                            }
                            else {
                                sets[item.itemset]++;
                                set = _.find(this.items.itemsets, {id: item.itemset})
                                if (set) {
                                    setbonus = _.get(set, "set"+sets[item.itemset]);
                                    if (setbonus)
                                        addStats(setbonus);
                                }
                            }
                        }
                    }
                }

                stats.intellect+= item_stats.int;
                stats.spirit+= item_stats.spi;
                stats.mp5+= item_stats.mp5;
                stats.spell_power+= item_stats.sp;
                stats.crit_rating = item_stats.crit;
                stats.hit_rating = item_stats.hit;
                stats.haste_rating = item_stats.haste;

                this.config.stats = stats;
            },

            itemConfig() {
                this.config.trinket1 = 0;
                this.config.trinket2 = 0;
                this.config.meta_gem = 0;
                if (this.isSpecialItem(this.equipped.trinket1))
                    this.config.trinket1 = this.equipped.trinket1;
                if (this.isSpecialItem(this.equipped.trinket2))
                    this.config.trinket2 = this.equipped.trinket2;
                if (this.metaGem() && this.isSpecialItem(this.metaGem().id) && this.isMetaGemActive())
                    this.config.meta_gem = this.metaGem().id;

                var num = this.numEquippedSet(this.items.ids.T10_SET);
                this.config.t10_2set = num > 1;
                this.config.t10_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T9_SET);
                this.config.t9_2set = num > 1;
                this.config.t9_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T8_SET);
                this.config.t8_2set = num > 1;
                this.config.t8_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T7_SET);
                this.config.t7_2set = num > 1;
                this.config.t7_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T6_SET);
                this.config.t6_2set = num > 1;
                this.config.t6_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T5_SET);
                this.config.t5_2set = num > 1;
                this.config.t5_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.T3_SET);
                this.config.t3_2set = num > 1;

                var num = this.numEquippedSet(this.items.ids.UDC_SET);
                this.config.udc_2set = num > 1;
                this.config.udc_3set = num > 2;
                this.config.udc_4set = num > 3;

                var num = this.numEquippedSet(this.items.ids.CUDC_SET);
                this.config.cudc_3set = num > 2;

                this.config.black_magic = this.enchants.weapon == this.items.ids.BLACK_MAGIC;
                this.config.lightweave_embroidery = this.enchants.back == this.items.ids.LIGHTWEAVE_EMBROIDERY;
                this.config.darkglow_embroidery = this.enchants.back == this.items.ids.DARKGLOW_EMBROIDERY;
                this.config.hyperspeed_accelerators = this.enchants.hands == this.items.ids.HYPERSPEED_ACCELERATORS;

                if (this.isEquipped("finger", this.items.ids.ASHEN_BAND_ENDLESS_DESTRUCTION) ||
                    this.isEquipped("finger", this.items.ids.ASHEN_BAND_UNMATCHED_DESTRUCTION) ||
                    this.isEquipped("finger", this.items.ids.ASHEN_BAND_ENDLESS_WISDOM) ||
                    this.isEquipped("finger", this.items.ids.ASHEN_BAND_UNMATCHED_WISDOM)
                )
                    this.config.ashen_band = true;
                else
                    this.config.ashen_band = false;

                this.config.rot_black_magic_ench = 0;
                if (this.config.rot_black_magic && !this.canBlackMagicWeave)
                    this.config.rot_black_magic = false;
                if (this.config.rot_black_magic)
                    this.config.rot_black_magic_ench = this.enchants.weapon;

            },

            simStats() {
                var x;
                this.itemStats();
                this.itemConfig();

                var stats = this.config.stats;

                // Custom stats
                for (var key in this.config.custom_stats) {
                    if (this.config.custom_stats[key])
                        stats[key]+= this.config.custom_stats[key];
                }

                // Arcane intellect
                stats.intellect+= 60;

                // Spirit
                if (this.config.divine_spirit)
                    stats.spirit+= 80;
                else if (this.config.fel_intelligence)
                    stats.spirit+= 64;

                if (this.config.mark_of_the_wild) {
                    x = 37;
                    if (this.config.imp_mark_of_the_wild)
                        x = 52;
                    stats.intellect+= x;
                    stats.spirit+= x;
                }

                // Flask
                if (this.config.flask) {
                    if (this.config.flask == this.flasks.FLASK_FROSTWYRM)
                        stats.spell_power+= 125;
                    else if (this.config.flask == this.flasks.FLASK_PURE_MOJO)
                        stats.mp5+= 45;
                }
                else {
                    // Guardian Elxir
                    if (this.config.guardian_elixir == this.elixirs.ELIXIR_SPIRIT)
                        stats.spirit+= 50;
                    else if (this.config.guardian_elixir == this.elixirs.ELIXIR_MIGHTY_THOUGHTS)
                        stats.intellect+= 45;
                    else if (this.config.guardian_elixir == this.elixirs.ELIXIR_MIGHTY_MAGEBLOOD)
                        stats.mp5+= 30;
                    else if (this.config.guardian_elixir == this.elixirs.ELIXIR_DRAENIC_WISDOM) {
                        stats.intellect+= 30;
                        stats.spirit+= 30;
                    }

                    // Battle elixir
                    if (this.config.battle_elixir == this.elixirs.ELIXIR_SPELLPOWER) {
                        stats.spell_power+= 58;
                    }
                    else if (this.config.battle_elixir == this.elixirs.ELIXIR_ACCURACY) {
                        stats.hit_rating+= 45;
                    }
                    else if (this.config.battle_elixir == this.elixirs.ELIXIR_DEADLY_STRIKES) {
                        stats.crit_rating+= 45;
                    }
                    else if (this.config.battle_elixir == this.elixirs.ELIXIR_LIGHTNING_SPEED) {
                        stats.haste_rating+= 45;
                    }
                    else if (this.config.battle_elixir == this.elixirs.ELIXIR_GURU) {
                        stats.intellect+= 20;
                        stats.spirit+= 20;
                    }
                }

                // Prof: Alchemy
                if (this.config.prof_alchemy) {
                    if (this.config.flask) {
                        if (this.config.flask == constants.flasks.FLASK_FROSTWYRM)
                            stats.spell_power+= 47;
                    }
                    else {
                        if (this.config.battle_elixir == constants.elixirs.ELIXIR_SPELLPOWER) {
                            stats.spell_power+= 23;
                        }
                        else if (this.config.battle_elixir == constants.elixirs.ELIXIR_ACCURACY) {
                            stats.hit_rating+= 20;
                        }
                        else if (this.config.battle_elixir == constants.elixirs.ELIXIR_DEADLY_STRIKES) {
                            stats.crit_rating+= 20;
                        }
                        else if (this.config.battle_elixir == constants.elixirs.ELIXIR_LIGHTNING_SPEED) {
                            stats.haste_rating+= 20;
                        }
                        else if (this.config.battle_elixir == constants.elixirs.ELIXIR_GURU) {
                            stats.intellect+= 8;
                            stats.spirit+= 8;
                        }

                        if (this.config.guardian_elixir == constants.elixirs.ELIXIR_SPIRIT)
                            stats.spirit+= 20;
                        else if (this.config.guardian_elixir == constants.elixirs.ELIXIR_MIGHTY_THOUGHTS)
                            stats.intellect+= 20;
                        else if (this.config.guardian_elixir == constants.elixirs.ELIXIR_MIGHTY_MAGEBLOOD)
                            stats.mp5+= 10;
                    }
                }

                // Food
                if (this.config.food == this.foods.FOOD_SPELL_POWER)
                    stats.spell_power+= 46;
                else if (this.config.food == this.foods.FOOD_HASTE)
                    stats.haste_rating+= 40;
                else if (this.config.food == this.foods.FOOD_HIT)
                    stats.hit_rating+= 40;
                else if (this.config.food == this.foods.FOOD_CRIT)
                    stats.crit_rating+= 40;

                // Focus magic
                if (this.config.focus_magic)
                    stats.crit+= 3;

                // Prof: Skinning
                if (this.config.prof_skinning)
                    stats.crit_rating+= 40;

                // Mana Restoration
                if (this.config.blessing_of_wisdom) {
                    x = 92;
                    if (this.config.imp_blessing_of_wisdom)
                        x = 110;
                    stats.mp5+= x;
                }
                else if (this.config.mana_spring) {
                    x = 91;
                    if (this.config.imp_blessing_of_wisdom)
                        x = 109;
                    stats.mp5+= x;
                }

                // Attribute multipliers
                if (this.config.talents.student_of_the_mind) {
                    var x = this.config.talents.student_of_the_mind;
                    if (x == 1) stats.spirit*= 1.04;
                    if (x == 2) stats.spirit*= 1.07;
                    if (x == 3) stats.spirit*= 1.1;
                }
                if (this.config.talents.arcane_mind)
                    stats.intellect*= 1.0 + this.config.talents.arcane_mind*0.03;
                if (this.config.race == this.races.RACE_GNOME)
                    stats.intellect*= 1.05;
                if (this.config.race == this.races.RACE_HUMAN)
                    stats.spirit*= 1.03;
                if (this.config.blessing_of_kings) {
                    stats.intellect*= 1.1;
                    stats.spirit*= 1.1;
                }
                if (this.config.drums_of_forgotten_kings) {
                    stats.intellect*= 1.08;
                    stats.spirit*= 1.08;
                }
                if (this.metaGem() && this.metaGem().id == this.items.ids.META_EMBER_SKYFLARE)
                    stats.intellect*= 1.02;
                stats.intellect = Math.round(stats.intellect);
                stats.spirit = Math.round(stats.spirit);

                if (this.config.race == this.races.RACE_DRAENEI || this.faction == "alliance" && this.config.heroic_presence)
                    stats.hit+= 1;
                stats.crit+= this.config.talents.arcane_instability;
                stats.crit+= this.config.talents.pyromaniac;
                stats.hit+= this.config.talents.precision;

                // Calculate percentages
                stats.crit+= stats.intellect/166.6667;
                stats.crit+= this.critRatingToChance(stats.crit_rating);
                stats.hit+= this.hitRatingToChance(stats.hit_rating);

                this.config.stats = stats;
            },

            displayStats() {
                var x;
                var stats = _.cloneDeep(this.config.stats);
                stats.mana = 3268;

                // Buff: Spell power
                if (this.config.demonic_pact || this.config.totem_of_wrath || this.config.flametongue) {
                    x = 0;
                    if (this.config.totem_of_wrath)
                        x = 280;
                    else if (this.config.flametongue)
                        x = 144;
                    if (this.config.demonic_pact && this.config.demonic_pact_bonus > x)
                        x = this.config.demonic_pact_bonus;
                    stats.spell_power+= x;
                }

                // Mind mastery
                if (this.config.talents.mind_mastery)
                    stats.spell_power+= Math.round(stats.intellect * this.config.talents.mind_mastery * 0.03);

                // Buff:: Spell haste
                if (this.config.buff_spell_haste)
                    stats.haste = this.multiplyHaste(stats.haste, 5);

                // Buff:: Haste
                if (this.config.buff_haste)
                    stats.haste = this.multiplyHaste(stats.haste, 3);

                // Buff:: Spell crit
                if (this.config.buff_spell_crit)
                    stats.crit+= 5;

                // Debuff: Spell crit
                if (this.config.debuff_spell_crit)
                    stats.crit+= 5;

                // Debuff: Spell hit
                if (this.config.debuff_spell_hit)
                    stats.hit+= 3;

                // Debuff: Crit
                if (this.config.debuff_crit)
                    stats.crit+= 3;

                if (this.config.molten_armor) {
                    var multi = 0.35;
                    if (this.config.glyphs.molten_armor)
                        multi+= 0.2;
                    if (this.numEquippedSet(this.items.ids.T9_SET) > 1)
                        multi+= 0.15;
                    var rating = Math.round(stats.spirit * multi);
                    stats.crit_rating+= rating;
                    stats.crit+= this.critRatingToChance(rating);
                }

                // Haste rating is a little special
                // We need to pass the raw haste rating to the sim, so we calculate the final percentage here
                stats.haste = this.multiplyHaste(stats.haste, this.hasteRatingToHaste(stats.haste_rating));

                // Mana
                stats.mana+= stats.intellect*15 - 280;
                if (this.metaGem() && this.metaGem().id == this.items.ids.META_BEAMING_EARTHSIEGE)
                    stats.mana = Math.round(stats.mana*1.02);

                this.display_stats = stats;
            },

            calcStats() {
                this.simStats();
                this.displayStats();
            },

            openItem(item) {
                var a = document.createElement("a");
                a.href = this.itemUrl(item.id);
                a.target = "_blank";
                a.click();
            },

            itemUrl(id) {
                if (typeof(id) == "object")
                    id = id.id;
                if (id > 99900)
                    return null;
                if (this.item_source == "wotlkdb")
                    return "https://www.wotlkdb.com/?item="+id;
                if (this.item_source == "evo")
                    return "https://wotlk.evowow.com/?item="+id;
                return "https://www.wowhead.com/wotlk/item="+id;
            },

            spellUrl(id) {
                if (typeof(id) == "object")
                    id = id.id;
                if (this.item_source == "wotlkdb")
                    return "https://www.wotlkdb.com/?spell="+id;
                if (this.item_source == "evo")
                    return "https://wotlk.evowow.com/?spell="+id;
                return "https://www.wowhead.com/wotlk/spell="+id;
            },

            equippedUrl(slot) {
                if (!this.equipped[slot])
                    return null;
                var url = this.itemUrl(this.equipped[slot]);
                var item = this.getItem(slot, this.equipped[slot]);

                var sockets = this.slotSockets(slot);
                if (sockets.length) {
                    var gems = [];
                    for (var i=0; i<sockets.length; i++)
                        gems.push(this.gems[slot][i] ? this.gems[slot][i] : "");
                    url+= "&gems="+gems.join(":");
                }

                if (this.hasExtraSocket(slot))
                    url+="&sock=1";

                if (_.get(this.enchants, slot)) {
                    var enchant = this.getEnchant(slot, this.enchants[slot]);
                    if (enchant)
                        url+= "&ench="+enchant.enchantmentId;
                }

                if (item.itemset) {
                    var pcs = [];
                    for (var key in this.equipped) {
                        var itm = this.getItem(key, this.equipped[key]);
                        if (_.get(itm, "itemset") == item.itemset)
                            pcs.push(itm.id);
                    }
                    if (pcs.length)
                        url+= "&pcs="+pcs.join(":");
                }

                return url;
            },

            critRatingToChance(rating) {
                return rating / 45.91;
            },

            hitRatingToChance(rating) {
                return rating / 26.232;
            },

            hasteRatingToHaste(rating) {
                return rating / 32.79;
            },

            multiplyHaste(h1, h2) {
                return (1 + h1/100) * (1 + h2/100) * 100 - 100;
            },

            isSpecialItem(item_id) {
                for (var key in this.items.ids) {
                    if (this.items.ids[key] == item_id)
                        return true;
                }
                return false;
            },

            onUnequip(slot) {
                if (slot == "neck") {
                    if (this.equipped[slot] == this.items.ids.EYE_OF_THE_NIGHT)
                        this.config.eye_of_the_night = false;
                    if (this.equipped[slot] == this.items.ids.CHAIN_OF_THE_TWILIGHT_OWL)
                        this.config.chain_of_the_twilight_owl = false;
                    if (this.equipped[slot] == this.items.ids.JADE_PENDANT_OF_BLASTING)
                        this.config.jade_pendant_of_blasting = false;
                    this.saveCurrentProfile();
                }

                if (this.equipped[slot] && this.gems[slot]) {
                    this.item_gems[this.equipped[slot]] = this.gems[slot];
                }

                if (slot == "weapon" && this.equipped.off_hand) {
                    this.item_off_hand = this.equipped.off_hand;
                }
            },

            equipToggle(slot, item) {
                if (this.equipped[slot] == item.id)
                    this.unequip(slot);
                else
                    this.equip(slot, item);
            },

            unequip(slot, save) {
                this.onUnequip(slot);
                this.equipped[slot] = null;
                this.gems[slot] = [null, null, null];

                this.calcStats();
                if (typeof(save) == "undefined" || save)
                    this.saveCurrentProfile();
            },

            equip(slot, item, save) {
                if (!_.isObject(item))
                    item = this.getItem(slot, item);

                if (this.equipped[slot] == item.id)
                    return;

                if (slot == "off_hand") {
                    var weapon = this.equippedItem("weapon");
                    if (weapon.twohand)
                        return;
                    this.item_off_hand = item.id;
                }

                if (item.unique) {
                    var other = this.otherSlot(slot);
                    if (other != slot && this.equipped[other]) {
                        if (this.isEquipped(other, item.id))
                            return;
                        // A unique category
                        if (item.unique !== true) {
                            var other_item = this.getItem(other, this.equipped[other]);
                            if (_.get(other_item, "unique", null) === item.unique)
                                return;
                        }
                    }
                }

                this.onUnequip(slot);

                if (slot == "weapon") {
                    if (item.twohand)
                        this.equipped.off_hand = null;
                    else if (!this.equipped.off_hand && this.item_off_hand)
                        this.equipped.off_hand = this.item_off_hand;

                    var enchant = this.equippedEnchant(slot);
                    if (!item.twohand && enchant && enchant.twohand)
                        this.enchants[slot] = 60714;
                    else if (item.twohand && enchant && !enchant.twohand)
                        this.enchants[slot] = 62948;
                }

                this.equipped[slot] = item.id;

                this.gems[slot] = [null, null, null];
                if (this.gems.hasOwnProperty(slot)) {
                    this.gems[slot] = this.defaultGems(slot, item);
                    if (this.item_gems.hasOwnProperty(item.id)) {
                        var n = this.slotSockets(slot).length;
                        for (var i=0; i<n; i++) {
                            if (i < this.item_gems[item.id].length)
                                this.gems[slot][i] = this.item_gems[item.id][i];
                        }
                    }
                    this.item_gems[item.id] = this.gems[slot];
                }

                this.calcStats();
                if (typeof(save) == "undefined" || save)
                    this.saveCurrentProfile();

                this.refreshTooltips();
            },

            isEquipped(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEquipped(slot+"1", id) || this.isEquipped(slot+"2", id);

                return _.get(this.equipped, slot) == id;
            },

            numEquippedSet(id) {
                var num = 0;

                for (var key in this.equipped) {
                    var slot = this.equipSlotToItemSlot(key);
                    var item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item && _.get(item, "itemset") == id)
                        num++;
                }

                return num;
            },

            enchant(slot, enchant) {
                if (enchant.twohand) {
                    var item = this.equippedItem(slot);
                    if (!item || !item.twohand)
                        return;
                }

                if (this.enchants[slot] == enchant.id)
                    this.enchants[slot] = null;
                else
                    this.enchants[slot] = enchant.id;

                this.saveCurrentProfile();
                this.calcStats();

                this.refreshTooltips();
            },

            isEnchanted(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEnchanted(slot+"1") || this.isEnchanted(slot+"2");

                return _.get(this.enchants, slot) == id;
            },

            setSocket(slot, gem, index) {
                if (this.isSocketed(slot, gem.id, index)) {
                    this.gems[slot].splice(index, 1, null);
                }
                else {
                    if (gem.unique && this.isSocketedAnywhere(gem.id, gem.unique))
                        return;
                    this.gems[slot].splice(index, 1, gem.id);
                }

                var item_id = this.equipped[slot];
                if (item_id) {
                    if (!this.item_gems.hasOwnProperty(item_id))
                        this.item_gems[item_id] = [null, null, null];
                    this.item_gems[item_id].splice(index, 1, gem.id);
                }

                this.saveCurrentProfile();
                this.calcStats();

                this.refreshTooltips();
            },

            quickset(set) {
                for (var slot in set.equip)
                    this.equipped[slot] = set.equip[slot];
                for (var slot in set.enchants)
                    this.enchants[slot] = set.enchants[slot];
                for (var slot in set.gems) {
                    this.gems[slot] = set.gems[slot];
                    if (this.equipped[slot])
                        this.item_gems[this.equipped[slot]] = this.gems[slot];
                }

                this.saveCurrentProfile();
                this.calcStats();
            },

            matchSocketColor(sock, gem) {
                if (gem == "a")
                    return true;
                if (sock == gem)
                    return true;
                if (sock == "r" && ["o", "p"].indexOf(gem) != -1)
                    return true;
                if (sock == "y" && ["o", "g"].indexOf(gem) != -1)
                    return true;
                if (sock == "b" && ["g", "p"].indexOf(gem) != -1)
                    return true;
                return false;
            },

            hasSocketBonus(slot, gems) {
                if (!gems)
                    gems = this.gems[slot];

                var item = this.equippedItem(slot);

                if (item && item.sockets && item.bonus) {
                    for (var i=0; i<item.sockets.length; i++) {
                        var gem_id = gems[i];
                        var gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                        if (!gem || !this.matchSocketColor(item.sockets[i], gem.color))
                            return false;
                    }

                    return true;
                }

                return false;
            },

            isSocketed(slot, id, index) {
                return _.get(this.gems[slot], index) == id;
            },

            isSocketedAnywhere(id, unique) {
                for (var slot in this.gems) {
                    for (var i=0; i<this.gems[slot].length; i++) {
                        if (id == this.gems[slot][i])
                            return true;
                        if (unique !== true) {
                            var gem = this.getGem(this.gems[slot][i]);
                            if (unique === _.get(gem, "unique", null))
                                return true;
                        }
                    }
                }
                return false;
            },

            metaGemHasCustomReq(meta) {
                return typeof(meta.req) == "string";
            },

            isMetaGemActive() {
                if (this.equipped.head && this.metaGem()) {
                    var meta = this.metaGem();
                    if (!meta.req)
                        return true;

                    var colors = {r: 0, b: 0, y: 0};
                    for (var slot in this.gems) {
                        if (this.equipped[slot]) {
                            for (var i in this.gems[slot]) {
                                if (this.gems[slot][i]) {
                                    var gem = this.getGem(this.gems[slot][i]);
                                    if (gem) {
                                        if (gem.color == "a") {
                                            colors.b++;
                                            colors.r++;
                                            colors.y++;
                                        }
                                        else if (gem.color == "o") {
                                            colors.r++;
                                            colors.y++;
                                        }
                                        else if (gem.color == "g") {
                                            colors.y++;
                                            colors.b++;
                                        }
                                        else if (gem.color == "p") {
                                            colors.r++;
                                            colors.b++;
                                        }
                                        else if (gem.color != "m") {
                                            colors[gem.color]++;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (!this.metaGemHasCustomReq(meta)) {
                        for (var color in meta.req) {
                            if (meta.req[color] > colors[color])
                                return false;
                        }
                    }
                    else if (meta.id == this.items.ids.MYSTICAL_SKYFIRE) {
                        return colors.b > colors.y;
                    }
                }
                return true;
            },

            metaGem() {
                for (var key in this.gems.head) {
                    if (this.gems.head[key]) {
                        var gem = _.find(this.items.gems, {id: this.gems.head[key]});
                        if (gem && gem.color == "m")
                            return gem;
                    }
                }

                return null;
            },

            defaultGems(slot, item) {
                var gems = [null, null, null];
                var sockets = this.slotSockets(slot, item);

                if (sockets.length) {
                    for (var i=0; i<sockets.length; i++)
                        gems[i] = this.defaultGem(sockets[i]);
                }

                return gems;
            },

            defaultGem(color) {
                if (color == "m")
                    return this.items.ids.META_CHAOTIC_SKYFLARE;
                return 39998; // 19 sp
            },

            confirmGems() {
                for (var slot in this.gems) {
                    var n = this.slotSockets(slot).length;
                    for (var i=n; i<3; i++) {
                        this.gems[slot][i] = null;
                    }
                    for (var i=0; i<this.gems[slot].length; i++) {
                        if (this.gems[slot][i]) {
                            var gem = this.getGem(this.gems[slot][i]);
                            this.gems[slot][i] = null;
                            if (gem) {
                                if (!gem.unique || !this.isSocketedAnywhere(gem.id, gem.unique))
                                    this.gems[slot][i] = gem.id;
                            }
                        }
                    }
                }
            },

            numDragonsEye() {
                var num = 0;
                var sockets, gem_id, gem;

                for (var key in this.gems) {
                    sockets = this.slotSockets(key);
                    if (sockets) {
                        for (var i=0; i<sockets.length; i++) {
                            gem_id = this.gems[key][i];
                            gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                            if (gem && gem.title.indexOf("Dragon's Eye") != -1)
                                num++;
                        }
                    }
                }

                return num;
            },

            numProfs() {
                var num = 0;
                if (this.config.prof_engineer || _.get(this.enchants, "hands") == this.items.ids.HYPERSPEED_ACCELERATORS || _.get(this.enchants, "back") == 63765 || _.get(this.enchants, "feet") == 55016)
                    num++;
                if (this.config.prof_skinning)
                    num++;
                if (this.config.prof_alchemy)
                    num++;
                if (this.config.wrist_socket || this.config.hands_socket)
                    num++;
                if (_.get(this.enchants, "shoulder") == 61120 || _.get(this.enchants, "shoulder") == 61118)
                    num++;
                if (_.get(this.enchants, "back") == this.items.ids.LIGHTWEAVE_EMBROIDERY || _.get(this.enchants, "back") == this.items.ids.DARKGLOW_EMBROIDERY)
                    num++;
                if (this.numDragonsEye())
                    num++;
                if (_.get(this.enchants, "finger1") || _.get(this.enchants, "finger2"))
                    num++;

                return num;
            },

            hasUseTrinket(nr) {
                var slot = "trinket"+nr;
                var item = this.equippedItem(slot);

                if (item && item.use)
                    return true;

                return false;
            },

            paperClick(slot, what) {
                this.setActiveSlot(slot);

                if (what == "enchant") {
                    this.$nextTick(function() {
                        this.scrollToEnchant();
                    });
                }
                else if (what == "gems") {
                    this.$nextTick(function() {
                        this.scrollToGems();
                    });
                }
                else {
                    this.$nextTick(function() {
                        this.scrollToItems();
                    });
                }
            },

            scrollToItems() {
                if (this.$refs.enchantAnchor && this.$refs.items) {
                    this.$refs.items.scrollTo(0, 0);
                }
            },

            scrollToEnchant() {
                if (this.$refs.enchantAnchor && this.$refs.items) {
                    this.$refs.items.scrollTo({
                        top: this.$refs.enchantAnchor.offsetTop - this.$refs.items.offsetTop - 30,
                        behavior: "smooth"
                    });
                }
            },

            scrollToGems() {
                if (this.$refs.gemsAnchor && this.$refs.items) {
                    this.$refs.items.scrollTo({
                        top: this.$refs.gemsAnchor.offsetTop - this.$refs.items.offsetTop - 30,
                        behavior: "smooth"
                    });
                }
            },

            updatePin() {
                if (this.result && this.result.avg_dps)
                    this.pin_dps = this.result.avg_dps;
            },

            removePin() {
                this.pin_dps = null;
            },

            isComparing(item) {
                return _.findIndex(this.item_comparison, {id: item.id}) != -1;
            },

            compareAll() {
                if (this.active_slot == "quicksets")
                    return;

                if (this.item_comparison.length == this.activeItems.length && _.find(this.item_comparison, {id: this.activeItems[0].id})) {
                    this.item_comparison = [];
                }
                else {
                    this.item_comparison = [];
                    for (var i in this.activeItems)
                        this.item_comparison.push({id: this.activeItems[i].id, dps: null});
                }
            },

            compareItem(item) {
                var index = _.findIndex(this.item_comparison, {id: item.id});
                if (index == -1)
                    this.item_comparison.push({id: item.id, dps: null});
                else
                    this.item_comparison.splice(index, 1);
            },

            comparisonDps(item) {
                var cmp = _.find(this.item_comparison, {id: item.id});
                return cmp && cmp.dps ? _.round(cmp.dps, 2) : null;
            },

            comparisonMetaGemInactive(item) {
                var cmp = _.find(this.item_comparison, {id: item.id});
                return cmp && !cmp.is_meta_gem_active;
            },

            setSpec(spec) {
                if (spec == "arcane") {
                    this.config.build = "https://www.wowhead.com/wotlk/talent-calc/mage/230005133100330150323102505321-03-023203001_001wr211q1b21q1y31rj441rj551rj7";
                    this.config.rotation = constants.rotations.ROTATION_ST_AB_AM;
                }
                else if (spec == "arcane_barrage") {
                    this.config.build = "https://www.wowhead.com/wotlk/talent-calc/mage/230005133100330150323102505321-03-023203001_001wr211q1b21q1y31rj441rj551rj7";
                    this.config.rotation = constants.rotations.ROTATION_ST_AB_AM_BARRAGE;
                }
                else if (spec == "fire") {
                    this.config.build = "https://www.wowhead.com/wotlk/talent-calc/mage/23000503110003-0055030012303331053120301351_001q1g11xkk21q1y31rj441rj551rj7";
                    this.config.rotation = constants.rotations.ROTATION_ST_FIRE;
                }
                else if (spec == "frost") {
                    this.config.build = "https://www.wowhead.com/wotlk/talent-calc/mage/23000503110003--0533030310233100030152231351_00258s11q1j21q1y31kd841rj451rj5";
                    this.config.rotation = constants.rotations.ROTATION_ST_FROST;
                }
                else if (spec == "ffb") {
                    this.config.build = "https://www.wowhead.com/wotlk/talent-calc/mage/-2305030012303331053120311351-023303031002_001vrn11xkk21q1y31rj441rj551rj6";
                    this.config.rotation = constants.rotations.ROTATION_ST_FROSTFIRE;
                }

                this.parseTalents();
            },

            onBuildInput() {
                this.parseTalents();
            },

            resetTalents() {
                for (var key in this.config.talents)
                    this.config.talents[key] = 0;
                for (var key in this.config.glyphs)
                    this.config.glyphs[key] = false;
            },

            parseTalents() {
                this.resetTalents();
                var m;
                if (m = this.config.build.match(/talent\#o(.*)/i))
                    this.parseEvoTalents(m[1]);
                else if (m = this.config.build.match(/talent-calc\/mage\/(.*)/i))
                    this.parseWowheadTalents(m[1]);
                else
                    alert("Unknown talent calculator");
            },

            parseWowheadTalents(build) {
                var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                var arr = build.split("_");
                var has_glyphs = arr.length > 1;
                var t = arr[0];
                var ch, value;
                var tree = 0, talent = 0;

                // Reset current talents
                for (var key in this.config.talents)
                    this.config.talents[key] = 0;

                for (var i=0; i<t.length; i++) {
                    ch = t.charAt(i);
                    if (ch == "-") {
                        tree++;
                        talent = 0;
                    }
                    else {
                        if (this.talent_map[tree][talent]) {
                            value = parseInt(ch);
                            this.config.talents[this.talent_map[tree][talent]] = value;
                        }
                        talent++;
                    }
                }

                if (has_glyphs) {
                    var g = arr[1];
                    var id, key, glyph;
                    for (var i=1; i < g.length; i+= 5) {
                        id = (encoding.indexOf(g[i + 1]) << 15) | (encoding.indexOf(g[i + 2]) << 10) | (encoding.indexOf(g[i + 3]) << 5) | (encoding.indexOf(g[i + 4]) << 0);
                        glyph = _.find(glyphs, {spellId: id});
                        if (glyph) {
                            key = glyph.name.replace("Glyph of ", "").replace(/ /g, "_").toLowerCase();
                            if (this.config.glyphs.hasOwnProperty(key))
                                this.config.glyphs[key] = true;
                        }
                    }
                }
            },

            parseEvoTalents(build) {
                var encoding = "0zMcmVokRsaqbdrfwihuGINALpTjnyxtgevElBCDFHJKOPQSUWXYZ123456789";
                var tree = 0, talent = 0;
                var values = [];
                var ch, n;
                var has_glyphs = false;

                var trees = [
                    [2,3,5,3,2,5,2,3,3,1,2,2,3,3,2,1,5,3,3,2,3,1,3,2,5,1,5,3,2,1],
                    [2,3,5,5,2,3,2,3,1,2,3,2,3,3,3,1,2,5,3,1,2,2,3,2,1,3,5,1],
                    [3,5,3,3,2,3,3,3,1,3,2,3,3,1,3,3,2,3,2,1,5,2,2,3,1,3,5,1],
                ];

                for (var i=0; i<build.length; i++) {
                    ch = build.charAt(i);

                    if (ch == ":") {
                        has_glyphs = true;
                        i++;
                        break;
                    }
                    else if (ch != "Z") {
                        n = encoding.indexOf(ch);
                        if (n < 0)
                            continue;

                        values[1] = n % 6;
                        values[0] = (n - values[1]) / 6;

                        for (var j=0; j<2; j++) {
                            n = Math.min(values[j], trees[tree][talent]);
                            if (this.talent_map[tree][talent])
                                this.config.talents[this.talent_map[tree][talent]] = n;

                            if (++talent >= trees[tree].length)
                                break;
                        }
                    }

                    if (talent >= trees[tree].length || ch == "Z") {
                        talent = 0;
                        if (++tree >= trees.length) {
                            if (build.charAt(i+1) == ":") {
                                has_glyphs = true;
                                i+=2;
                            }
                            break;
                        }
                    }
                }

                if (has_glyphs) {
                    var slot = 0;
                    var glyph, key;
                    var g = [[],[],[]];
                    var fnSort = function(a, b) { return a.itemId - b.itemId };
                    g[1] = glyphs.filter(a => a.type == 1).sort(fnSort);
                    g[2] = glyphs.filter(a => a.type == 2).sort(fnSort);
                    for (i; i<build.length; i++) {
                        ch = build.charAt(i);
                        if (ch == "Z") {
                            slot = 3;
                        }
                        else {
                            glyph = g[slot > 2 ? 2 : 1][encoding.indexOf(ch)];
                            key = glyph.name.replace("Glyph of ", "").replace(/ /g, "_").toLowerCase();
                            if (this.config.glyphs.hasOwnProperty(key))
                                this.config.glyphs[key] = true;
                            slot++;
                        }
                    }
                }
            },

            formatStats(item) {
                var stats = [];

                if (item.sp)
                    stats.push(item.sp+" sp");
                if (item.hit)
                    stats.push(item.hit+" hit");
                if (item.crit)
                    stats.push(item.crit+" crit");
                if (item.haste)
                    stats.push(item.haste+" haste");
                if (item.int)
                    stats.push(item.int+" int");
                if (item.spi)
                    stats.push(item.spi+" spi");
                if (item.mp5)
                    stats.push(item.mp5+" mp5");
                if (item.desc)
                    stats.push(item.desc);

                return stats.join(" / ");
            },

            formatKey(str) {
                return _.startCase(str);
            },

            formatSockets(item) {
                if (!item.sockets)
                    return null;

                return item.sockets.join(" / ");
            },

            formatTime(s) {
                var sign = s < 0 ? "-" : "";
                s = Math.abs(s);
                var dec = Math.round((s%1) * 100);
                var sec = Math.floor(s);
                if (dec == 100) {
                    dec = 0;
                    sec++;
                }

                return sign+(sec < 10 ? "0"+sec : sec)+"."+(dec < 10 ? "0"+dec : dec);
            },

            formatTimeDiff(start, end) {
                return this.$round(Math.abs(start.getTime() - end.getTime())/1000, 2)+"s";
            },

            formatDateTime(date) {
                return date.toLocaleTimeString("sv");
            },

            round(num) {
                return Math.round(num);
            },

            errorNotice(title, text) {
                this.error_notice.open = true;
                this.error_notice.title = title;
                this.error_notice.text = text;

                var self = this;
                clearTimeout(this.error_notice.timeout);
                this.error_notice.timeout = setTimeout(function() {
                    self.error_notice.open = false;
                }, 10000);
            },


            /**
             * Sim export/import
             */
            exportString() {
                var data = {
                    equipped: this.export_profile.items ? _.cloneDeep(this.equipped) : null,
                    enchants: this.export_profile.items ? _.cloneDeep(this.enchants) : null,
                    gems: this.export_profile.items ? _.cloneDeep(this.gems) : null,
                    config: this.export_profile.config ? _.cloneDeep(this.config) : null,
                };

                return btoa(JSON.stringify(data));
            },

            checkImportString() {
                try {
                    var json = atob(this.import_profile.string);
                    if (!json)
                        return;
                }
                catch (e) {
                    return;
                }

                try {
                    var data = JSON.parse(json);
                }
                catch (e) {
                    return;
                }

                if (!data)
                    return;

                this.import_status.items = _.get(data, "equipped", null) !== null;
                this.import_status.config = _.get(data, "config", null) !== null;
            },

            importString(str) {
                var import_type = null;
                try {
                    var data = JSON.parse(str);
                    if (data.phase)
                        import_type = "80up";
                    else if (data.gear && data.gear.items)
                        import_type = "wse";
                }
                catch (e) {
                    import_type = "native";
                }

                if (import_type == "80up")
                    return this.importEightyUpgradesString(str);
                else if (import_type == "wse")
                    return this.importWSEString(str);
                else if (import_type != "native")
                    return this.importError("Could not parse import string");

                try {
                    var json = atob(str);
                    if (!json)
                        return this.importError("Could not parse import string");
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                try {
                    var data = JSON.parse(json);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.equipped && !data.enchants && !data.gems && !data.config)
                    return this.importError("Invalid import string");

                if (!this.import_profile.items)
                    data.equipped = data.enchants = data.gems = null;
                if (!this.import_profile.config)
                    data.config = null;

                this.loadProfile(data);

                return true;
            },

            importError(err) {
                alert(err);
                this.import_profile.string = null;
                this.$refs.import_input.focus();
                return false;
            },

            doImport() {
                if (this.import_profile.string && this.importString(this.import_profile.string))
                    this.closeImport();
            },

            openExport() {
                this.export_profile.string = this.exportString();
                this.export_profile.open = true;

                this.$nextTick(function() {
                    this.$refs.export_input.select();
                });
            },

            updateExport() {
                var self = this;
                setTimeout(function() {
                    self.export_profile.string = self.exportString();

                    self.$nextTick(function() {
                        self.$refs.export_input.select();
                    });
                }, 100);
            },

            openImport() {
                this.import_profile.string = null;
                this.import_profile.open = true;

                this.$nextTick(function() {
                    this.$refs.import_input.focus();
                });
            },

            closeExport() {
                this.export_profile.open = false;
                this.export_profile.string = null;
            },

            closeImport() {
                this.import_profile.open = false;
                this.import_profile.string = null;
            },


            /**
             * Eighty upgrades import
             */
            importEightyUpgradesString(str) {
                try {
                    var data = JSON.parse(str);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.items)
                    return this.importError("Invalid import string");

                var profile = {
                    items: null,
                    enchants: null,
                    gems: null,
                    config: _.cloneDeep(this.config),
                };

                var errors = [];

                if (this.import_profile.items) {
                    profile.equipped = {};
                    profile.enchants = {};
                    profile.gems = {};

                    for (var key in this.equipped) {
                        profile.equipped[key] = null;
                        profile.enchants[key] = null;
                        profile.gems[key] = [null, null, null];
                    }

                    for (var i=0; i<data.items.length; i++) {
                        var slot = this.getSlotFromEightyUpgrades(data.items[i]);
                        if (!slot)
                            continue;
                        var item = this.getItem(slot, data.items[i].id);
                        if (!item)
                            item = this.searchItem(data.items[i].name);
                        if (!item) {
                            errors.push(data.items[i].name);
                            continue;
                        }
                        profile.equipped[slot] = item.id;

                        if (data.items[i].enchant) {
                            var enchant = this.getEnchantFromEightyUpgrades(slot, data.items[i].enchant);
                            if (!enchant)
                                errors.push(data.items[i].enchant.name);
                            else
                                profile.enchants[slot] = enchant.id;
                        }

                        if (data.items[i].gems) {
                            var nsockets = _.get(item, "sockets.length", 0);
                            for (var j=0; j<data.items[i].gems.length; j++) {
                                if (!data.items[i].gems[j])
                                    continue;
                                var gem = this.getGem(data.items[i].gems[j].id);
                                if (!gem)
                                    gem = this.searchGem(data.items[i].gems[j].name);

                                if (j >= nsockets) {
                                    if (j == nsockets && ["waist", "wrist", "hands"].indexOf(slot) != -1)
                                        profile.config[slot+"_socket"] = true;
                                    else
                                        break;
                                }

                                if (!gem) {
                                    var str = data.items[i].gems[j].name;
                                    if (errors.indexOf(str) == -1)
                                        errors.push(str);
                                }
                                else {
                                    profile.gems[slot][j] = gem.id;
                                }
                            }
                        }
                    }
                }

                // 80up has not implemented this yet
                if (this.import_profile.config && data.exportOptions.buffs) {

                }

                if (this.import_profile.config && data.exportOptions.talents) {
                    var talents = [
                        "000000000000000000000000000000",
                        "0000000000000000000000000000",
                        "0000000000000000000000000000",
                    ];

                    var tmap = {
                        "Arcane Subtlety": [0, 0],
                        "Arcane Focus": [0, 1],
                        "Arcane Stability": [0, 2],
                        "Arcane Fortitude": [0, 3],
                        "Magic Absorption": [0, 4],
                        "Arcane Concentration": [0, 5],
                        "Magic Attunement": [0, 6],
                        "Spell Impact": [0, 7],
                        "Student of the Mind": [0, 8],
                        "Focus Magic": [0, 9],
                        "Arcane Shielding": [0, 10],
                        "Improved Counterspell": [0, 11],
                        "Arcane Meditation": [0, 12],
                        "Torment of the Weak": [0, 13],
                        "Torment the Weak": [0, 13],
                        "Improved Blink": [0, 14],
                        "Presence of Mind": [0, 15],
                        "Arcane Mind": [0, 16],
                        "Prismatic Cloak": [0, 17],
                        "Arcane Instability": [0, 18],
                        "Arcane Potency": [0, 19],
                        "Arcane Empowerment": [0, 20],
                        "Arcane Power": [0, 21],
                        "Incanter's Absorption": [0, 22],
                        "Arcane Flows": [0, 23],
                        "Mind Mastery": [0, 24],
                        "Slow": [0, 25],
                        "Missile Barrage": [0, 26],
                        "Netherwind Presence": [0, 27],
                        "Spell Power": [0, 28],
                        "Arcane Barrage": [0, 29],
                        "Improved Fire Blast": [1, 0],
                        "Incineration": [1, 1],
                        "Improved Fireball": [1, 2],
                        "Ignite": [1, 3],
                        "Burning Determination": [1, 4],
                        "World in Flames": [1, 5],
                        "Flame Throwing": [1, 6],
                        "Impact": [1, 7],
                        "Pyroblast": [1, 8],
                        "Burning Soul": [1, 9],
                        "Improved Scorch": [1, 10],
                        "Molten Shields": [1, 11],
                        "Master of Elements": [1, 12],
                        "Playing with Fire": [1, 13],
                        "Critical Mass": [1, 14],
                        "Blast Wave": [1, 15],
                        "Blazing Speed": [1, 16],
                        "Fire Power": [1, 17],
                        "Pyromaniac": [1, 18],
                        "Combustion": [1, 19],
                        "Molten Fury": [1, 20],
                        "Fiery Payback": [1, 21],
                        "Empowered Fire": [1, 22],
                        "Firestarter": [1, 23],
                        "Dragon's Breath": [1, 24],
                        "Hot Streak": [1, 25],
                        "Burnout": [1, 26],
                        "Living Bomb": [1, 27],
                        "Frostbite": [2, 0],
                        "Improved Frostbolt": [2, 1],
                        "Ice Floes": [2, 2],
                        "Ice Shards": [2, 3],
                        "Frost Warding": [2, 4],
                        "Precision": [2, 5],
                        "Permafrost": [2, 6],
                        "Piercing Ice": [2, 7],
                        "Icy Veins": [2, 8],
                        "Improved Blizzard": [2, 9],
                        "Arctic Reach": [2, 10],
                        "Frost Channeling": [2, 11],
                        "Shatter": [2, 12],
                        "Cold Snap": [2, 13],
                        "Improved Cone of Cold": [2, 14],
                        "Frozen Core": [2, 15],
                        "Cold as Ice": [2, 16],
                        "Winter's Chill": [2, 17],
                        "Shattered Barrier": [2, 18],
                        "Ice Barrier": [2, 19],
                        "Arctic Winds": [2, 20],
                        "Empowered Frostbolt": [2, 21],
                        "Fingers of Frost": [2, 22],
                        "Brain Freeze": [2, 23],
                        "Summon Water Elemental": [2, 24],
                        "Enduring Winter": [2, 25],
                        "Chilled to the Bone": [2, 26],
                        "Deep Freeze": [2, 27],
                    };

                    for (var talent of data.talents) {
                        if (!tmap.hasOwnProperty(talent.name))
                            return this.importError("Unknown talent: "+talent.name);
                        var t = tmap[talent.name]
                        talents[t[0]] = talents[t[0]].substr(0, t[1]) + talent.rank + talents[t[0]].substr(t[1]+1);
                    }

                    var tstring = talents[0]+"-"+talents[1]+"-"+talents[2];
                    tstring = tstring.replace(/0+\-/g, "-");
                    tstring = tstring.replace(/0+$/g, "");
                    tstring = "https://wowhead.com/wotlk/talent-calc/mage/"+tstring;

                    if (data.glyphs && data.glyphs.length) {
                        var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                        var glyph, str, id;
                        tstring+= "_0";
                        for (var i=0, n=0; i<data.glyphs.length; i++, n++) {
                            glyph = _.find(glyphs, {itemId: data.glyphs[i].id});
                            if (!glyph)
                                continue;
                            if (data.glyphs[i].type == "MINOR" && i < 3)
                                n = 3;
                            id = glyph.spellId;
                            str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                            tstring+= n + str;
                        }
                    }

                    profile.config.build = tstring;
                }

                if (errors.length) {
                    errors.unshift("Following item(s) could not be found:");
                    this.errorNotice("Warning", errors);
                }

                this.loadProfile(profile);

                return true;
            },

            getSlotFromEightyUpgrades(data) {
                var slot = _.isString(data) ? data : data.slot;
                slot = slot.toLowerCase();
                slot = slot.replace("finger_", "finger");
                slot = slot.replace("trinket_", "trinket");
                slot = slot.replace("shoulders", "shoulder");
                slot = slot.replace("wrists", "wrist");
                if (slot == "main_hand")
                    slot = "weapon";
                if (!this.equipped.hasOwnProperty(slot))
                    return null;
                return slot;
            },

            getEnchantFromEightyUpgrades(slot, data) {
                if (!data.spellId) {
                    if (data.itemId) {
                        var map = {
                            28886: 35406,
                            28909: 35437,
                            23545: 29467,
                            28881: 35405,
                            28903: 35436,
                            29191: 35447,
                            19787: 24164,
                            24274: 31372,
                            24273: 31371
                        };
                        if (map.hasOwnProperty(data.itemId))
                            return this.getEnchant(slot, map[data.itemId]);
                        return this.searchEnchant(slot, data.name);
                    }
                    else {
                        return this.searchEnchant(slot, data.name);
                    }
                }

                var enchant = this.getEnchant(slot, data.spellId);
                if (!enchant)
                    enchant = this.searchEnchant(slot, data.name);
                return enchant;
            },


            /**
             * WowSims Exporter
             */
            importWSEString(str) {
                try {
                    var data = JSON.parse(str);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.gear || !data.gear.items)
                    return this.importError("Invalid import string");

                var profile = {
                    items: null,
                    enchants: null,
                    gems: null,
                    config: _.cloneDeep(this.config),
                };

                var errors = [];

                if (this.import_profile.items) {
                    profile.equipped = {};
                    profile.enchants = {};
                    profile.gems = {};

                    for (var key in this.equipped) {
                        profile.equipped[key] = null;
                        profile.enchants[key] = null;
                        profile.gems[key] = [null, null, null];
                    }

                    var slots = [
                        "head", "neck", "shoulder", "back", "chest",
                        "wrist", "hands", "waist", "legs", "feet",
                        "finger1", "finger2", "trinket1", "trinket2",
                        "weapon", "off_hand", "ranged",
                    ];

                    for (var i=0; i<data.gear.items.length; i++) {
                        if (data.gear.items[i]) {
                            var slot = slots[i];
                            var item = this.getItem(slot, data.gear.items[i].id);
                            if (!item) {
                                errors.push(data.gear.items[i].id);
                                continue;
                            }
                            profile.equipped[slot] = data.gear.items[i].id;

                            if (data.gear.items[i].enchant) {
                                var enchant = this.getEnchantFromEnchantmentId(slot, data.gear.items[i].enchant);
                                if (!enchant)
                                    errors.push(data.gear.items[i].enchant);
                                else
                                    profile.enchants[slot] = enchant.id;
                            }

                            if (data.gear.items[i].gems) {
                                var nsockets = _.get(item, "sockets.length", 0);
                                for (var j=0; j<data.gear.items[i].gems.length; j++) {
                                    var gem = this.getGem(data.gear.items[i].gems[j]);

                                    if (j >= nsockets) {
                                        if (j == nsockets && ["waist", "wrist", "hands"].indexOf(slot) != -1)
                                            profile.config[slot+"_socket"] = true;
                                        else
                                            break;
                                    }

                                    if (!gem) {
                                        var str = data.gear.items[i].gems[j];
                                        if (errors.indexOf(str) == -1)
                                            errors.push(str);
                                    }
                                    else {
                                        profile.gems[slot][j] = gem.id;
                                    }
                                }
                            }
                        }
                    }
                }

                if (this.import_profile.config && data.race) {
                    var race = data.race.toLowerCase().replace(/\s+/, "");
                    if (race == "undead")
                        profile.config.race = constants.races.RACE_UNDEAD;
                    else if (race == "troll")
                        profile.config.race = constants.races.RACE_TROLL;
                    else if (race == "bloodelf")
                        profile.config.race = constants.races.RACE_BLOOD_ELF;
                    else if (race == "draenei")
                        profile.config.race = constants.races.RACE_DRAENEI;
                    else if (race == "gnome")
                        profile.config.race = constants.races.RACE_GNOME;
                    else if (race == "human")
                        profile.config.race = constants.races.RACE_HUMAN;
                }

                if (this.import_profile.config && data.talents) {
                    var tstring = "https://wowhead.com/wotlk/talent-calc/mage/"+data.talents;
                    var rotation = null;

                    if (data.glyphs) {
                        var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                        var glyph, str, id;
                        tstring+= "_0";

                        if (data.glyphs.major) {
                            for (var i=0, n=0; i<data.glyphs.major.length; i++, n++) {
                                glyph = _.find(glyphs, {name: data.glyphs.major[i]});
                                if (!glyph)
                                    continue;
                                if (glyph.spellId == 56363)
                                    rotation = constants.rotations.ROTATION_ST_AB_AM;
                                else if (glyph.spellId == 56368)
                                    rotation = constants.rotations.ROTATION_ST_FIRE;
                                else if (glyph.spellId == 61205)
                                    rotation = constants.rotations.ROTATION_ST_FROSTFIRE;
                                else if (glyph.spellId == 56370)
                                    rotation = constants.rotations.ROTATION_ST_FROST;
                                id = glyph.spellId;
                                str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                                tstring+= n + str;
                            }
                        }

                        if (data.glyphs.minor) {
                            for (var i=0, n=3; i<data.glyphs.minor.length; i++, n++) {
                                glyph = _.find(glyphs, {name: data.glyphs.minor[i]});
                                if (!glyph)
                                    continue;
                                id = glyph.spellId;
                                str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                                tstring+= n + str;
                            }
                        }
                    }

                    if (rotation && rotation != this.config.rotation) {
                        var main_rotations = [
                            constants.rotations.ROTATION_ST_AB_AM,
                            constants.rotations.ROTATION_ST_FIRE,
                            constants.rotations.ROTATION_ST_FROSTFIRE,
                            constants.rotations.ROTATION_ST_FROST,
                        ];

                        if (main_rotations.indexOf(this.config.rotation) != -1)
                            profile.config.rotation = rotation;
                    }

                    profile.config.build = tstring;
                }

                if (errors.length) {
                    errors.unshift("Following item(s) could not be found:");
                    this.errorNotice("Warning", errors);
                }

                this.loadProfile(profile);

                return true;
            },


            nukeSettings() {
                if (!window.confirm("This will remove all profiles and configurations from this computer"))
                    return;

                localStorage.clear();
                window.location.reload(true);
            },

            saveHistory() {
                var profile = {
                    id: "",
                    name: "",
                    equipped: _.cloneDeep(this.equipped),
                    enchants: _.cloneDeep(this.enchants),
                    gems: _.cloneDeep(this.gems),
                    config: _.cloneDeep(this.config),
                }
                var str = JSON.stringify(profile);

                // Check if config is the same as last run
                if (this.history.length) {
                    var p = _.cloneDeep(this.history[0]);
                    p.id = p.name = "";
                    delete p.date;
                    delete p.end;
                    delete p.result;
                    if (JSON.stringify(p) == JSON.stringify(profile))
                        return false;
                }

                profile.date = new Date;
                profile.end = null;
                profile.result = null;

                this.history.unshift(profile);

                if (this.history.length > 30)
                    this.history.pop();

                return true;
            },

            loadHistory(profile) {
                var p = _.cloneDeep(profile);
                delete p.result;
                delete p.date;
                this.loadProfile(p);
            },

            toggleFavorite(item) {
                var id = _.isObject(item) ? item.id : item;
                var index = this.favorites.indexOf(id);
                if (index == -1)
                    this.favorites.push(id);
                else
                    this.favorites.splice(index, 1);
                this.saveFavorites();
            },

            isFavorite(item) {
                var id = _.isObject(item) ? item.id : item;
                return this.favorites.indexOf(id) != -1;
            },

            loadFavorites() {
                var str = window.localStorage.getItem("magesim_wotlk_favorites");
                if (!str)
                    return;

                var favorites = JSON.parse(str);
                if (!favorites || !Array.isArray(favorites))
                    return;

                this.favorites = favorites;
            },

            saveFavorites() {
                window.localStorage.setItem("magesim_wotlk_favorites", JSON.stringify(this.favorites));
            },

            getRotationString(rot) {
                if (rot == this.rotations.ROTATION_ST_FROSTFIRE)
                    return "Frostfire";
                if (rot == this.rotations.ROTATION_ST_FIRE || rot == this.rotations.ROTATION_ST_FIRE_SC)
                    return "Fire";
                if (rot == this.rotations.ROTATION_ST_FROST)
                    return "Frost";
                if (rot == this.rotations.ROTATION_ST_AB_AM || rot == this.rotations.ROTATION_ST_AB_AM_BARRAGE)
                    return "Arcane";
                if (rot == this.rotations.ROTATION_AOE_AE)
                    return "Arcane Explosion";
                if (rot == this.rotations.ROTATION_AOE_BLIZZ)
                    return "Blizzard";
                if (rot == this.rotations.ROTATION_AOE_BLIZZ_FS)
                    return "Blizzard + FS";
                if (rot == this.rotations.ROTATION_AOE_FIRE)
                    return "AoE Fire";
                return "Unknown";
            },

            moveProfile(index, dir) {
                var pos = (this.profiles.length + index + dir) % this.profiles.length;
                this.profiles.splice(pos, 0, this.profiles.splice(index, 1)[0]);
                this.saveProfiles();
            },

            saveProfile(profile, confirm) {
                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1 && !window.confirm("Override "+profile.name+"?") && !confirm)
                    return;
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.gems = _.cloneDeep(this.gems);
                profile.config = _.cloneDeep(this.config);

                if (index != -1)
                    this.profiles.splice(index, 1, profile);
                else
                    this.profiles.push(profile);

                this.saveProfiles();
            },

            loadProfile(profile, only) {
                this.profile_status.open = true;
                this.profile_status.items = false;
                this.profile_status.config = false;
                this.profile_status.missing_items = [];

                if (profile.equipped && (!only || only == "items")) {
                    profile.equipped = _.pick(profile.equipped, _.keys(this.equipped));
                    delete profile.equipped.stat_weight;
                    for (var slot in profile.equipped) {
                        if (profile.equipped[slot] && !this.getItem(slot, profile.equipped[slot])) {
                            profile.equipped[slot] = null;
                            this.profile_status.missing_items.push(this.equipSlotToItemSlot(slot));
                        }
                    }
                    _.merge(this.equipped, profile.equipped);
                    this.profile_status.items = true;
                }

                if (profile.enchants && (!only || only == "items")) {
                    profile.enchants = _.pick(profile.enchants, _.keys(this.enchants));
                    for (var slot in profile.enchants) {
                        if (!this.getEnchant(slot, profile.enchants[slot]))
                            profile.enchants[slot] = null;
                    }
                    _.merge(this.enchants, profile.enchants);
                }

                if (profile.gems && (!only || only == "items")) {
                    profile.gems = _.pick(profile.gems, _.keys(this.gems));
                    for (var slot in profile.gems) {
                        for (var i in profile.gems[slot]) {
                            if (!this.getGem(profile.gems[slot][i]))
                                profile.gems[slot][i] = null;
                        }
                    }
                    _.merge(this.gems, profile.gems);
                }

                if (profile.config && (!only || only == "config")) {
                    var config = _.cloneDeep(this.default_config);
                    this.config.timings = [];
                    this.config.interruptions = [];
                    _.merge(config, _.pick(profile.config, _.keys(config)));
                    _.merge(this.config, config);
                    this.onLoadConfig(profile.config);
                    this.profile_status.config = true;
                }

                this.confirmGems();
                this.calcStats();
                this.saveCurrentProfile();

                var self = this;
                clearTimeout(this.profile_status.timeout);
                this.profile_status.timeout = setTimeout(function() {
                    self.profile_status.open = false;
                }, 4000);
            },

            deleteProfile(profile) {
                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1) {
                    this.profiles.splice(index, 1);
                    this.saveProfiles();
                }
            },

            newProfile() {
                if (!this.new_profile)
                    return;

                var profile = {
                    id: this.uuid(),
                    name: this.new_profile,
                    equipped: {},
                    enchants: {},
                    gems: {},
                    config: {},
                };

                this.new_profile = null;

                this.saveProfile(profile);
            },

            uuid() {
                return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
                    var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
                    return v.toString(16);
                });
            },

            copyEquiplist() {
                var arr = [];

                var str, item, enchant, gem;
                for (var slot in this.equipped) {
                    if (!this.equipped[slot])
                        continue;
                    item = this.getItem(slot, this.equipped[slot]);
                    str = this.formatKey(slot)+": "+item.title;

                    if (_.get(this.enchants, slot)) {
                        enchant = this.getEnchant(slot, this.enchants[slot]);
                        str+= " ("+enchant.title+")";
                    }

                    if (_.get(this.gems, slot)) {
                        for (var i in this.gems[slot]) {
                            if (this.gems[slot][i]) {
                                gem = this.getGem(this.gems[slot][i]);
                                str+= " ["+gem.title+"]";
                            }
                        }
                    }

                    arr.push(str);
                }

                str = arr.join("\r\n");
                this.$copyToClipboard(str);
            },

            openEquiplist() {
                this.equiplist_open = true;
            },

            closeEquiplist() {
                this.equiplist_open = false;
            },

            openCustomStats() {
                this.custom_stats_open = true;
            },

            closeCustomStats() {
                this.custom_stats_open = false;
            },

            openCustomItem() {
                var slot = this.equipSlotToItemSlot(this.active_slot);
                if (slot != "quicksets")
                    this.custom_item.slot = slot;
                this.custom_item_open = true;
            },

            closeCustomItem() {
                this.custom_item.id = null;
                this.custom_item.title = null;
                this.custom_item.slot = null;
                this.custom_item.sockets = null;
                this.custom_item.int = null;
                this.custom_item.spi = null;
                this.custom_item.sp = null;
                this.custom_item.crit = null;
                this.custom_item.hit = null;
                this.custom_item.haste = null;
                this.custom_item.mp5 = null;
                this.custom_item.twohand = false;
                this.custom_item_open = false;
                this.custom_item_error = null;
            },

            addCustomItem() {
                this.custom_item_error = null;

                try {
                    if (!this.custom_item.slot)
                        throw "Choose a slot";
                    if (!this.custom_item.title)
                        throw "Enter a title";
                    if (this.custom_item.id && this.findItem(this.custom_item.id))
                        throw "Item id already exists";
                }
                catch(e) {
                    this.custom_item_error = e;
                    return;
                }

                var item = _.clone(this.custom_item);
                item.custom = true;

                if (!item.id)
                    item.id = this.createItemId();

                item.sockets = [];
                if (item.slot == "head" && item.meta_socket)
                    item.sockets.push("m");
                for (var i=0; i<this.custom_item.sockets; i++)
                    item.sockets.push("a");

                if (item.slot != "weapon" || !item.twohand)
                    delete item.twohand;
                delete item.meta_socket;
                delete item.slot;

                for (var key in item) {
                    if (!item[key])
                        delete item[key];
                }

                this.items.equip[this.custom_item.slot].push(item);
                this.saveCustomItems();
                this.closeCustomItem();

                this.$nextTick(function() {
                    this.refreshTooltips();
                });
            },

            deleteCustomItem(item) {
                var slot = this.equipSlotToItemSlot(this.active_slot);
                var index = _.findIndex(this.items.equip[slot], {id: item.id});
                if (index != -1) {
                    if (this.isEquipped(this.active_slot, item.id))
                        this.unequip(this.active_slot);
                    this.items.equip[slot].splice(index, 1);
                    this.saveCustomItems();
                    this.refreshTooltips();
                }
            },

            customItems() {
                var items = {};

                for (var slot in this.items.equip) {
                    var arr = this.items.equip[slot].filter(item => item.custom);
                    if (arr.length)
                        items[slot] = arr;
                }

                return items;
            },

            createItemId() {
                var id;
                while (true) {
                    id = 100000 + Math.round(Math.random() * 500000);
                    if (!this.findItem(id))
                        return id;
                }
            },

            setTab(name) {
                if (this.active_tab == name)
                    this.active_tab = "gear";
                else
                    this.active_tab = name;

                if (this.active_tab == "gear")
                    this.refreshTooltips();
            },

            allResults(result) {
                if (!result)
                    result = this.result;
                var a = document.createElement("a");
                a.href = "data:text/csv,"+encodeURIComponent(result.all_results);
                a.download = "simdata.csv";
                a.click();
            },

            refreshTooltips() {
                if (window.$WowheadPower) {
                    window.$WowheadPower.refreshLinks();
                    this.$nextTick(function() {
                        window.$WowheadPower.refreshLinks();
                    });
                }
            },

            saveCurrentProfile() {
                var profile = {};
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.gems = _.cloneDeep(this.gems);
                profile.config = _.cloneDeep(this.config);
                window.localStorage.setItem("magesim_wotlk_profile", JSON.stringify(profile));
            },

            loadCurrentProfile() {
                var str = window.localStorage.getItem("magesim_wotlk_profile");
                if (!str)
                    return;

                var profile = JSON.parse(str);
                if (!profile)
                    return;

                this.loadProfile(profile);
            },

            onLoadConfig(cfg) {
                if (this.config.conjured == constants.potions.POTION_FLAME_CAP)
                    this.config.conjured = constants.conjureds.CONJURED_NONE;
                this.parseTalents();
            },

            loadDefaultProfiles() {
                var prof;
                for (var i=0; i<default_profiles.length; i++) {
                    prof = JSON.parse(atob(default_profiles[i].str));
                    prof.name = default_profiles[i].name;
                    prof.id = "default-profile-"+i;
                    prof.icon = default_profiles[i].icon ? default_profiles[i].icon : null;
                    this.default_profiles.push(prof);
                }
            },

            saveProfiles() {
                window.localStorage.setItem("magesim_wotlk_profiles", JSON.stringify(this.profiles));
            },

            loadProfiles() {
                var str = window.localStorage.getItem("magesim_wotlk_profiles");
                if (str) {
                    var profiles = JSON.parse(str);
                    if (profiles)
                        this.profiles = profiles;
                }
            },

            saveCustomItems() {
                window.localStorage.setItem("magesim_wotlk_custom_items", JSON.stringify(this.customItems()));
            },

            loadCustomItems() {
                var str = window.localStorage.getItem("magesim_wotlk_custom_items");
                if (str) {
                    var items = JSON.parse(str);
                    if (items) {
                        for (var slot in items) {
                            for (var i=0; i<items[slot].length; i++)
                                this.items.equip[slot].push(items[slot][i]);
                        }
                    }
                }
            },
        }
    }
</script>
