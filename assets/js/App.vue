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
                                <td>Intellect</td>
                                <td>{{ display_stats.intellect }}</td>
                            </tr>
                            <tr>
                                <td>Spirit</td>
                                <td>{{ display_stats.spirit }}</td>
                            </tr>
                            <tr>
                                <td>Mp5</td>
                                <td>{{ display_stats.mp5 }}</td>
                            </tr>
                            <tr>
                                <td>Spell power</td>
                                <td>{{ display_stats.spell_power }}</td>
                            </tr>
                            <tr>
                                <td>Crit</td>
                                <td>
                                    <span>{{ $round(display_stats.crit, 2) }}%</span>
                                    <tooltip position="r">{{ display_stats.crit_rating }} crit rating</tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td>Hit</td>
                                <td>
                                    <span>{{ $round(display_stats.hit, 2) }}%</span>
                                    <tooltip position="r">{{ display_stats.hit_rating }} hit rating</tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td>Haste</td>
                                <td>
                                    <span>{{ $round(display_stats.haste, 2) }}%</span>
                                    <tooltip position="r">{{ display_stats.haste_rating }} haste rating</tooltip>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
                <div class="ep-stats" v-if="epCalc">
                    <div class="title">
                        <span>Stat weights</span>
                        <help>
                            Stat weights are calculated by running {{ config.iterations }} iterations with +10 of each stat with the same RNG seed and comparing the dps gain.<br>
                            Calculated stat weights are based on your config. Any changes to it or your items can change the weights.<br>
                            The best way to find out if an item/gem/enchant is better is to equip it and run simulations.
                        </help>
                    </div>
                    <select v-model="ep_weight">
                        <option value="dps">DPS</option>
                        <option value="int">Intellect (EP)</option>
                        <option value="spi">Spirit (EP)</option>
                        <option value="mp5">Mp5 (EP)</option>
                        <option value="sp">Spell power (EP)</option>
                        <option value="crit">Crit rating (EP)</option>
                        <option value="hit">Hit rating (EP)</option>
                        <option value="haste">Haste rating (EP)</option>
                    </select>
                    <table class="simple mt-1">
                        <tbody>
                            <tr @click="ep_weight = 'int'">
                                <td>Intellect</td>
                                <td>{{ $nullRound(epCalc.int, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'spi'">
                                <td>Spirit</td>
                                <td>{{ $nullRound(epCalc.spi, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'mp5'">
                                <td>Mp5</td>
                                <td>{{ $nullRound(epCalc.mp5, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp'">
                                <td>Spell power</td>
                                <td>{{ $nullRound(epCalc.sp, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp_crit'">
                                <td>Crit rating</td>
                                <td>{{ $nullRound(epCalc.crit, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'spt_hit'">
                                <td>Hit rating</td>
                                <td>{{ $nullRound(epCalc.hit, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp_haste'">
                                <td>Haste rating</td>
                                <td>{{ $nullRound(epCalc.haste, 2) }}</td>
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
                            <div>{{ $round(result.avg_dps, 2) }}</div>
                            <div class="faded">{{ $round(result.min_dps, 2) }} - {{ $round(result.max_dps, 2) }}</div>
                        </div>
                        <div class="faded" v-if="result.stats.evocated.n">
                            Evocated: {{ $round(result.stats.evocated.t, 1) }}s
                            ({{ $round(result.stats.evocated.n / result.iterations * 100, 1) }}%)
                        </div>
                        <div class="faded" v-if="result.stats.t_gcd_capped">
                            Wasted haste: {{ $round(result.stats.t_gcd_capped, 2) }}s
                            <help>Time spent gcd capped</help>
                        </div>
                        <div class="btn mt-1" :class="[is_running ? 'disabled' : '']" @click="findAvg(result.avg_dps)">Find average fight</div>
                        <div class="btn mt-1" v-if="result.all_results" @click="allResults">Simulation data</div>
                    </template>
                    <template v-else>
                        <div class="dps-result">
                            <div>DPS</div>
                            <div>{{ $round(result.dps, 2) }}</div>
                            <div>Damage: {{ result.dmg }}</div>
                        </div>
                        <div class="mt-1"></div>
                        <div class="faded" v-if="result.evocated_at > 0">Evocated at: {{ $round(result.evocated_at, 1) }}</div>
                        <div class="faded" v-if="result.t_gcd_capped">
                            Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s
                            <help>Time spent gcd capped</help>
                        </div>
                    </template>
                    <template v-if="!isMetaGemActive()">
                        <div class="meta-warning mt-2">
                            <span>
                                <span class="material-icons">&#xe002;</span>
                                <tooltip position="right">Meta gem requirements have not been met.</tooltip>
                            </span>
                        </div>
                    </template>
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
                    <div class="gear" v-if="active_tab == 'gear'">
                        <div class="slots">
                            <div
                                class="slot"
                                :class="[active_slot == slot ? 'active' : '']"
                                v-for="slot in slots"
                                @click="setActiveSlot(slot);"
                            >{{ formatKey(slot) }}</div>
                        </div>
                        <div class="items">
                            <div class="items-wrapper">
                                <div class="top clearfix">
                                    <div class="fl clearfix">
                                        <div class="form-item">
                                            <select v-model="phase_filter">
                                                <option :value="0">- Filter by content phase -</option>
                                                <option :value="1">Phase 1 - Naxxramas</option>
                                                <option :value="2">Phase 2 - Ulduar</option>
                                                <option :value="3">Phase 3 - TotC, Onyxia</option>
                                                <option :value="4">Phase 4 - ICC</option>
                                                <option :value="5">Phase 5 - Ruby Sanctum</option>
                                            </select>
                                        </div>
                                        <div class="form-item">
                                            <input type="text" v-model="search_item" placeholder="Search...">
                                        </div>
                                    </div>
                                    <div class="fr">
                                        <div class="btn" :class="[!hasComparisons || is_running ? 'disabled' : '']" @click="runComparison">
                                            Run item comparison
                                        </div>
                                        <div class="btn" @click="openEquiplist">
                                            Equipped items overview
                                        </div>
                                        <div class="btn" @click="openCustomItem">
                                            Add custom item
                                        </div>
                                    </div>
                                </div>

                                <table class="large mt-2">
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
                                                <sort-link v-model="item_sort" name="phase">Phase</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="sockets" order="desc">Sockets</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="sp" order="desc">Spell power</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="crit" order="desc">Crit rating</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="hit" order="desc">Hit rating</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="haste" order="desc">Haste rating</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="int" order="desc">Intellect</sort-link>
                                            </th>
                                            <th>
                                                <sort-link v-model="item_sort" name="spi" order="desc">Spirit</sort-link>
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
                                                <span class="delete" @click.stop="deleteCustomItem(item)" v-if="$get(item, 'custom')">
                                                    <help icon="e872">Delete custom item</help>
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
                                            </td>
                                            <td v-if="hasComparisons">
                                                {{ comparisonDps(item) }}
                                            </td>
                                            <td>{{ $get(item, "phase", 1) }}</td>
                                            <td>
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
                                        <tr
                                            class="item"
                                            @click="quickset(set)"
                                            v-for="(set, key) in items.quicksets"
                                            v-if="active_slot == 'quicksets'"
                                        >
                                            <td></td>
                                            <td></td>
                                            <td>{{ set.title }}</td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                            <td></td>
                                        </tr>
                                    </tbody>
                                </table>

                                <table class="large mt-4" v-if="activeEnchants.length">
                                    <thead>
                                        <tr>
                                            <th>Enchant</th>
                                            <th>Spell power</th>
                                            <th>Crit rating</th>
                                            <th>Hit rating</th>
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
                                            <td>{{ $get(item, "int", "") }}</td>
                                            <td>{{ $get(item, "spi", "") }}</td>
                                            <td>{{ $get(item, "mp5", "") }}</td>
                                        </tr>
                                    </tbody>
                                </table>

                                <div class="mt-4"></div>

                                <div class="extra-socket mb-2" v-if="['hands', 'wrist'].indexOf(active_slot) != -1">
                                    <label>
                                        <input type="checkbox" v-model="config.hands_socket" v-if="active_slot == 'hands'">
                                        <input type="checkbox" v-model="config.wrist_socket" v-if="active_slot == 'wrist'">
                                        <span>Extra socket from blacksmithing</span>
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
                                                    <th>Gem</th>
                                                    <th>Stats</th>
                                                    <th v-if="socket == 'm'">Requires</th>
                                                    <th v-else>Unique</th>
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
                                                    <td>
                                                        <a :href="itemUrl(gem)" class="gem-color" :class="['color-'+gem.color]" target="_blank" @click.stop>
                                                            {{ gem.title }}
                                                        </a>
                                                    </td>
                                                    <td>{{ formatStats(gem) }}</td>
                                                    <td v-if="socket == 'm'">
                                                        <template v-if="gem.req">
                                                            <template v-if="metaGemHasCustomReq(gem)">
                                                                {{ gem.req }}
                                                            </template>
                                                            <template v-else>
                                                                <div class="socket-text-color" :class="['color-'+c]" v-for="(n, c) in gem.req">{{ n }}</div>
                                                            </template>
                                                        </template>
                                                    </td>
                                                    <td v-else><template v-if="gem.unique">Yes</template></td>
                                                </tr>
                                            </tbody>
                                        </table>
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
                                    <label><input type="checkbox" v-model="log_filter[9]"> <span>Show GCD cap</span></label>
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
                                    <tr v-for="spell in result.spells">
                                        <td>{{ spell.source }}</td>
                                        <td>{{ spell.name }}</td>
                                        <td>{{ spell.casts }} ({{ $round(spell.casts / numCasts * 100, 1) }}%)</td>
                                        <td>{{ spell.misses }} ({{ $round(spell.misses/spell.casts*100, 2) }}%)</td>
                                        <td>{{ spell.hits }}</td>
                                        <td>{{ spell.crits }} ({{ $round(spell.crits/spell.casts*100, 2) }}%)</td>
                                        <td>{{ $round(spell.dmg, 0) }} ({{ $round(spell.dmg / result.dmg * 100, 2) }}%)</td>
                                        <td>{{ $round(spell.min_dmg, 0) }}</td>
                                        <td>{{ $round(spell.dmg / (spell.casts - spell.misses), 0) }}</td>
                                        <td>{{ $round(spell.max_dmg, 0) }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="histog" v-if="active_tab == 'histogram'">
                        <histogram :data="result.histogram"></histogram>
                    </div>

                    <div class="config" v-if="active_tab == 'config'">
                        <div class="fieldsets">
                            <fieldset class="config-general">
                                <legend>General</legend>
                                <div class="form-item">
                                    <label>Quick spec</label>
                                    <span class="btn secondary" @click="setSpec('arcane')">Arcane</span>
                                    <span class="btn secondary" @click="setSpec('arcane_barrage')">Barrage</span>
                                    <span class="btn secondary" @click="setSpec('fire')">Fire</span>
                                    <span class="btn secondary" @click="setSpec('frost')">Frost</span>
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
                                <div class="form-item form-row">
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
                                        <label>Resistance</label>
                                        <input type="text" v-model.number="config.target_resistance">
                                    </div>
                                </div>
                                <div class="form-item">
                                    <label>No. of targets</label>
                                    <input type="text" v-model.number="config.targets">
                                </div>
                                <div class="form-item">
                                    <label>Spell travel time (ms)</label>
                                    <input type="text" v-model.number="config.spell_travel_time">
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
                                        <option :value="rotations.ROTATION_ST_FROST">Frost</option>
                                        <option :value="rotations.ROTATION_AOE_AE">Arcane Explosion</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="config.talents.imp_scorch">
                                    <label><input type="checkbox" v-model="config.maintain_imp_scorch">
                                        <span>Keep up imp. scorch</span>
                                        <help>Imp. Scorch from you</help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="[rotations.ROTATION_ST_AB_AM, rotations.ROTATION_ST_AB_AM_BARRAGE].indexOf(config.rotation) != -1">
                                    <label><input type="checkbox" v-model="config.rot_ab_stacks_three">
                                        <span>Only stack Arcane Blast to 3</span>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.rotation == rotations.ROTATION_ST_FROST">
                                    <label><input type="checkbox" v-model="config.rot_ice_lance">
                                        <span>Ice Lance at end of Fingers of Frost</span>
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
                                    <label>Drums</label>
                                    <select v-model="config.drums">
                                        <option :value="drums.DRUMS_NONE">None</option>
                                        <option :value="drums.DRUMS_OF_BATTLE">Drums of Battle (80 haste)</option>
                                        <option :value="drums.DRUMS_OF_WAR">Drums of War (30 sp)</option>
                                        <option :value="drums.DRUMS_OF_RESTORATION">Drums of Restoration (600 mana)</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="config.drums">
                                    <label><input type="checkbox" v-model="config.drums_friend">
                                        <span>Drumming friend</span>
                                        <help>Someone else in your raid uses drums</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label>Potion</label>
                                    <select v-model="config.potion">
                                        <option :value="potions.POTION_NONE">None</option>
                                        <option :value="potions.POTION_MANA">Mana potion</option>
                                        <option :value="potions.POTION_SPEED">Potion of Speed</option>
                                        <option :value="potions.POTION_WILD_MAGIC">Potion of Wild Magic</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Conjured</label>
                                    <select v-model="config.conjured">
                                        <option :value="conjureds.CONJURED_NONE">None</option>
                                        <option :value="conjureds.CONJURED_FLAME_CAP">Flame Cap</option>
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
                                            <tr v-for="timing in config.timings" :key="timing.id" v-if="timingEnabled(timing.name)">
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
                                            </tr>
                                        </tbody>
                                    </table>
                                    <div class="add-timing">
                                        <div class="plus">
                                            <span class="material-icons">&#xe145;</span>
                                        </div>
                                        <div class="menu">
                                            <div v-for="timing in timings" v-if="timingEnabled(timing.name)" @click="addTiming(timing.name)">
                                                <img :src="timing.icon">
                                                <tooltip>{{ timing.title }}</tooltip>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-profiles">
                                <legend>Profiles</legend>
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
                                        <input type="text" v-model="new_profile" @keydown.enter="newProfile()">
                                        <div class="btn" :class="[new_profile ? '' : 'disabled']" @click="newProfile()">
                                            New profile
                                            <tooltip position="r">Save your items and config</tooltip>
                                        </div>
                                    </div>
                                    <div class="export-import clearfix mt-2">
                                        <div class="btn fl" @click="openExport()">Export</div>
                                        <div class="btn fl ml-n" @click="openImport()">Import</div>
                                        <div class="btn danger fr" @click="nukeSettings()">Nuke settings</div>
                                    </div>
                                </div>
                            </fieldset>
                        </div>
                    </div>
                </div>
            </div>

            <div class="lightbox small warning" v-if="beta_warning_open">
                <div class="closer" @click="closeBetaWarning"></div>
                <div class="inner">
                    <div class="title">BETA</div>
                    <div class="text">This sim is under construction. Results are not accurate.</div>
                    <div class="btn mt-2" @click="closeBetaWarning">I understand</div>
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
                    <div class="btn mt-2" @click="closeExport">Close</div>
                    <div class="close" @click="closeExport">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>
            </div>

            <div class="lightbox" v-if="import_profile.open">
                <div class="closer" @click="closeImport"></div>
                <div class="inner">
                    <div class="title">Import</div>
                    <div class="form-item">
                        <textarea v-model="import_profile.string" ref="import_input" @input="checkImportString"></textarea>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="import_profile.items" :disabled="!import_status.items"> <span>Include items</span></label>
                        <label><input type="checkbox" v-model="import_profile.config" :disabled="!import_status.config"> <span>Include config</span></label>
                    </div>
                    <div class="btn mt-2" :class="[import_profile.string ? '' : 'disabled']" @click="doImport">Import</div>
                    <div class="close" @click="closeImport">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
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
                            <tr
                                class="equipped-item"
                                v-for="(item_id, slot) in equipped"
                                v-if="item_id"
                            >
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
                                    <template v-if="gems.hasOwnProperty(slot)">
                                        <template v-for="(gem_id, index) in gems[slot]" v-if="gem_id">
                                            <span v-if="index > 0">,</span>
                                            <a :href="itemUrl(gem_id)" target="_blank" :class="['gem-color', 'color-'+getGem(gem_id).color]">
                                                {{ getGem(gem_id).title }}
                                            </a>
                                        </template>
                                    </template>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                    <div class="mt-2">
                        <div class="btn" @click="copyEquiplist">Copy</div>
                        <div class="btn" @click="closeEquiplist">Close</div>
                    </div>
                    <div class="close" @click="closeEquiplist">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
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
                            <label>Number of sockets</label>
                            <input type="number" v-model.number="custom_item.sockets">
                        </div>
                    </div>
                    <div class="mt-2 text-error" v-if="custom_item_error">
                        {{ custom_item_error }}
                    </div>
                    <div class="mt-2">
                        <div class="btn" @click="addCustomItem">Save</div>
                    </div>
                    <div class="close" @click="closeCustomItem">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
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

    export default {
        mounted() {
            this.loadCustomItems();
            this.loadCurrentProfile();
            this.loadProfiles();
            this.calcStats();

            this.checkDonation();
            this.betaWarning();
        },

        data() {
            var default_config = {
                iterations: 20000,
                race: 5,
                shatt_faction: 0,

                duration: 180,
                duration_variance: 0,
                rng_seed: 0,
                avg_spell_dmg: false,
                additional_data: false,
                targets: 1,
                target_resistance: 0,
                target_level: 83,
                spell_travel_time: 500,
                reaction_time: 300,

                // Debuffs
                debuff_crit: false,
                debuff_spell_crit: false,
                debuff_spell_hit: false,
                debuff_spell_dmg: false,
                judgement_of_wisdom: false,

                // Buffs
                mage_armor: false,
                molten_armor: true,
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

                // Consumes
                food: 0,
                flask: 0,
                battle_elixir: 0,
                guardian_elixir: 0,
                weapon_oil: 0,

                black_magic: false,
                lightweave_embroidery: false,
                darkglow_embroidery: false,
                hyperspeed_accelerators: false,
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

                drums_friend: false,
                pre_mirror_image: false,
                pre_water_elemental: false,

                wrist_socket: false,
                hands_socket: false,

                drums: constants.drums.DRUMS_NONE,
                potion: constants.potions.POTION_MANA,
                pre_potion: constants.potions.POTION_SPEED,
                conjured: constants.conjureds.CONJURED_NONE,

                meta_gem: 0,
                trinket1: 0,
                trinket2: 0,

                rotation: constants.rotations.ROTATION_ST_AB_AM,
                rot_ab_stacks_three: false,
                rot_ice_lance: false,

                timings: Array(),

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

                talents: {
                    arcane_focus: 0,
                    clearcast: 0,
                    spell_impact: 0,
                    student_of_the_mind: 0,
                    focus_magic: 0,
                    arcane_meditation: 0,
                    torment_of_the_weak: 0,
                    presence_of_mind: 0,
                    arcane_mind: 0,
                    arcane_instability: 0,
                    arcane_potency: 0,
                    arcane_empowerment: 0,
                    arcane_power: 0,
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
                beta_warning_open: false,
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
                profiles: [],
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
                custom_item: {
                    id: null,
                    title: null,
                    slot: null,
                    q: "rare",
                    sockets: null,
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
                equiplist_open: false,
                equiplist_string: null,
                display_stats: null,
                result: null,
                ep_result: null,
                ep_weight: "dps",
                is_running: false,
                is_running_ep: false,
                active_tab: "gear",
                item_source: "evo",
                phase_filter: 0,
                search_item: "",
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
                    "8": true,
                    "9": true,
                    "10": true,
                },
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

            data.slots = [...slots, "quicksets"];

            data.talent_map[0][1] = "arcane_focus";
            data.talent_map[0][5] = "clearcast";
            data.talent_map[0][7] = "spell_impact";
            data.talent_map[0][8] = "student_of_the_mind";
            data.talent_map[0][9] = "focus_magic";
            data.talent_map[0][12] = "arcane_meditation";
            data.talent_map[0][13] = "torment_of_the_weak";
            data.talent_map[0][15] = "presence_of_mind";
            data.talent_map[0][16] = "arcane_mind";
            data.talent_map[0][18] = "arcane_instability";
            data.talent_map[0][19] = "arcane_potency";
            data.talent_map[0][20] = "arcane_empowerment";
            data.talent_map[0][21] = "arcane_power";
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

            activeItems() {
                var slot = this.equipSlotToItemSlot(this.active_slot);

                var items = this.items.equip[slot];

                if (!items)
                    return [];

                if (this.phase_filter)
                    items = items.filter(item => _.get(item, "phase", 1) <= this.phase_filter);

                if (this.search_item)
                    items = items.filter(item => item.title.toLowerCase().indexOf(this.search_item.toLowerCase()) != -1);

                return this.sort(items, this.item_sort);
            },

            activeEnchants() {
                var slot = this.equipSlotToEnchantSlot(this.active_slot);

                if (!this.items.enchants.hasOwnProperty(slot))
                    return [];

                return this.items.enchants[slot];
            },

            activeSockets() {
                var item = this.equippedItem(this.active_slot);
                var sockets = [];
                if (item && item.sockets)
                    sockets = _.clone(item.sockets);
                if (this.active_slot == "wrist" && this.config.wrist_socket)
                    sockets.push("a");
                if (this.active_slot == "hands" && this.config.hands_socket)
                    sockets.push("a");
                return sockets;
            },

            activeLog() {
                if (!this.result || !this.result.log)
                    return [];

                var log = this.result.log;

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
                    int: null,
                    spi: null,
                    mp5: null,
                    sp: null,
                    crit: null,
                    hit: null,
                    haste: null,
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
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_bloodlust.jpg",
                });
                timings.push({
                    name: "power_infusion",
                    title: "Power Infusion",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_powerinfusion.jpg",
                });
                timings.push({
                    name: "mana_tide",
                    title: "Mana Tide",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/spell_frost_summonwaterelemental.jpg",
                });
                timings.push({
                    name: "innervate",
                    title: "Innervate",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "mana_gem",
                    title: "Mana Gem",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_misc_gem_sapphire_02.jpg",
                });
                timings.push({
                    name: "evocation",
                    title: "Evocation",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_nature_purge.jpg",
                });
                timings.push({
                    name: "berserking",
                    title: "Berserking",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/racial_troll_berserk.jpg",
                });
                timings.push({
                    name: "presence_of_mind",
                    title: "Presence of Mind",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_nature_enchantarmor.jpg",
                });
                timings.push({
                    name: "arcane_power",
                    title: "Arcane Power",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "combustion",
                    title: "Combustion",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_fire_sealoffire.jpg",
                });
                timings.push({
                    name: "icy_veins",
                    title: "Icy Veins",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_frost_coldhearted.jpg",
                });
                timings.push({
                    name: "cold_snap",
                    title: "Cold Snap",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_frost_wizardmark.jpg",
                });
                timings.push({
                    name: "hyperspeed_accelerators",
                    title: "Hyperspeed Accelerators",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_enggizmos_04.jpg",
                });
                timings.push({
                    name: "drums",
                    title: "Drums",
                    icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_misc_drum_02.jpg",
                });

                var potion = {
                    name: "potion",
                    title: "Runic Mana Potion",
                    icon: "https://wow.zamimg.com/images/wow/icons/large/inv_alchemy_elixir_02.jpg",
                };
                if (this.config.potion == constants.POTION_SPEED) {
                    potion.title = "Potion of Speed";
                    potion.icon = "https://wow.zamimg.com/images/wow/icons/large/inv_alchemy_elixir_04.jpg";
                }
                if (this.config.potion == constants.POTION_WILD_MAGIC) {
                    potion.title = "Potion of Wild Magic";
                    potion.icon = "https://wow.zamimg.com/images/wow/icons/large/inv_alchemy_elixir_01.jpg";
                }
                timings.push(potion);

                if (this.config.potion == constants.POTION_WILD_MAGIC) {
                    timings.push({
                        name: "conjured",
                        title: "Flame Cap",
                        icon: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_herb_flamecap.jpg",
                    });
                }

                var trinkets = [
                    {
                        id: this.items.ids.TRINKET_TWILIGHT_SERPENT,
                        title: "Figurine - Twilight Serpent",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_jewelcrafting_purpleserpent.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_TOME_ARCANE_PHENOMENA,
                        title: "Tome of Arcane Phenomena",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_misc_book_07.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_CANNONEERS_FUSELIGHTER,
                        title: "Cannoneer's Fuselighter",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_gizmo_elementalblastingpowder.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_MARK_WAR_PRISONER,
                        title: "Mark of the War Prisoner",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_jewelry_talisman_13.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_NAARU_SLIVER,
                        title: "Shifting Naaru Sliver",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_jewelry_talisman_15.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SKULL_GULDAN,
                        title: "Skull of Gul'dan",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_misc_bone_elfskull_01.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_SHRUNKEN_HEAD,
                        title: "Hex Shrunken Head",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/inv_misc_head_troll_01.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_MQG,
                        title: "Mind Quickening Gem",
                        icon: "https://wotlk.evowow.com/static/images/wow/icons/large/spell_nature_wispheal.jpg",
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

                if (this.enchants.weapon == this.items.ids.BLACK_MAGIC)
                    buffs.push({id: constants.buffs.BLACK_MAGIC, name: "Black Magic"});
                if (this.enchants.back == this.items.ids.LIGHTWEAVE_EMBROIDERY)
                    buffs.push({id: constants.buffs.LIGHTWEAVE, name: "Lightweave Embroidery"});

                if (this.numEquippedSet(this.items.ids.T8_SET) > 1)
                    buffs.push({id: constants.buffs.PRAXIS, name: "Praxis (t8 2set)"});
                if (this.numEquippedSet(this.items.ids.T10_SET) > 1)
                    buffs.push({id: constants.buffs.PUSHING_THE_LIMIT, name: "Pushing the Limit (t10 2set)"});

                buffs = _.sortBy(buffs, "name");

                return buffs;
            }
        },

        methods: {
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
                    "innervate", "mana_gem", "evocation",
                ];
                if (always.indexOf(name) != -1)
                    return true;

                if (name == "potion")
                    return this.config.potion != constants.potions.POTION_NONE;
                if (name == "conjured")
                    return this.config.conjured != constants.conjureds.CONJURED_NONE;
                if (name == "drums")
                    return this.config.drums != constants.drums.DRUMS_NONE;
                if (name == "hyperspeed_accelerators")
                    return this.enchants.hands == this.items.ids.HYPERSPEED_ACCELERATORS;
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
                if (name == "trinket1")
                    return this.equipped.trinket1 && _.get(this.equippedItem("trinket1"), "use");
                if (name == "trinket2")
                    return this.equipped.trinket2 && _.get(this.equippedItem("trinket2"), "use");

                return false;
            },

            timingCanWait(name) {
                var nowait = ["bloodlust", "mana_tide", "power_infusion", "innervate"];
                if (nowait.indexOf(name) != -1)
                    return false;

                if (name == "drums")
                    return !this.config.drums_friend;

                return true;
            },

            sort(items, sorting) {
                if (!sorting || !sorting.name)
                    return items;

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

                var self = this;

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

            betaWarning() {
                if (!localStorage.getItem("beta_warning")) {
                    this.beta_warning_open = true;
                }
            },

            closeBetaWarning() {
                localStorage.setItem("beta_warning", 1);
                this.beta_warning_open = false;
            },

            runMultiple() {
                var self = this;
                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                if (["log", "timeline", "spells"].indexOf(this.active_tab) != -1)
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                this.is_running = true;
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

                var addStats = function(config, stats) {
                    stats = _.merge({
                        int: 0,
                        spi: 0,
                        mp5: 0,
                        crit: 0,
                        hit: 0,
                        sp: 0,
                        haste: 0,
                    }, stats);

                    config.stats.intellect+= stats.int;
                    config.stats.spirit+= stats.spi;
                    config.stats.mp5+= stats.mp5;
                    config.stats.spell_power+= stats.sp;
                    config.stats.crit+= self.critRatingToChance(stats.crit);
                    config.stats.hit+= self.hitRatingToChance(stats.hit);
                    config.stats.haste+= self.hasteRatingToHaste(stats.haste);
                };

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
                    var config = _.cloneDeep(self.config);
                    if (rng_seed)
                        config.rng_seed = rng_seed;
                    if (value)
                        addStats(config, {[stat]: value});
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
                    int: null,
                    spi: null,
                    mp5: null,
                    sp: null,
                    crit: null,
                    hit: null,
                    haste: null,
                };

                var rng_seed = Math.round(Math.random() * 100000);
                var result;
                for (var stat in this.ep_result) {
                    result = await this.runStat(stat, stat == "base" ? 0 : 10, rng_seed);
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

            setActiveSlot(slot) {
                if (this.is_running)
                    return;

                this.active_slot = slot;
                this.item_comparison = [];

                if (window.$WowheadPower) {
                    this.$nextTick(function() {
                        this.refreshTooltips();
                    });
                }
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

            getGem(id) {
                return _.find(this.items.gems, {id: id}, null);
            },

            getEnchant(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {id: id}, null);
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

            activeGems(index) {
                if (this.activeSockets.length < index)
                    return [];

                if (this.activeSockets[index] == "m")
                    var gems = this.items.gems.filter(g => g.color == "m");
                else
                    var gems = this.items.gems.filter(g => g.color != "m");

                if (this.phase_filter)
                    gems = gems.filter(g => _.get(g, "phase", 1) <= this.phase_filter);
                if (this.search_gem)
                    gems = gems.filter(g => g.title.toLowerCase().indexOf(this.search_gem.toLowerCase()) != -1);

                return gems;
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
                    stats.intellect = 185;
                    stats.spirit = 173;
                }
                if (this.config.race == this.races.RACE_DRAENEI) {
                    stats.intellect = 182;
                    stats.spirit = 176;
                }
                if (this.config.race == this.races.RACE_GNOME) {
                    stats.intellect = 193;
                    stats.spirit = 174;
                }
                if (this.config.race == this.races.RACE_HUMAN) {
                    stats.intellect = 181;
                    stats.spirit = 179;
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

                var slot, item, has_bonus, get_bonus, enchant, gem, gem_id, set, setbonus;
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

                        if (item.sockets) {
                            for (var i=0; i<item.sockets.length; i++) {
                                gem_id = this.gems[key][i];
                                gem = gem_id ? _.find(this.items.gems, {id: gem_id}) : null;
                                if (gem && (gem.color != "m" || this.isMetaGemActive()))
                                    addStats(gem);
                                if (has_bonus && (!gem || !this.matchSocketColor(item.sockets[i], gem.color)))
                                    get_bonus = false;
                            }

                            if (has_bonus && get_bonus)
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

                if (this.numEquippedSet(this.items.ids.DUSKWEAVE_SET))
                    item_stats.sp+= 18;

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

                this.config.black_magic = this.enchants.weapon == this.items.ids.BLACK_MAGIC;
                this.config.lightweave_embroidery = this.enchants.back == this.items.ids.LIGHTWEAVE_EMBROIDERY;
                this.config.darkglow_embroidery = this.enchants.back == this.items.ids.DARKGLOW_EMBROIDERY;
                this.config.hyperspeed_accelerators = this.enchants.hands == this.items.ids.HYPERSPEED_ACCELERATORS;
            },

            simStats() {
                var x;
                this.itemStats();
                this.itemConfig();

                var stats = this.config.stats;

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

                // Item sets
                var num = this.numEquippedSet(this.items.ids.GLADIATOR_SET);
                if (num > 1)
                    stats.spell_power+= 29;
                if (num > 3)
                    stats.spell_power+= 88;

                // Attribute multipliers
                if (this.config.talents.student_of_the_mind) {
                    if (x == 1) stats.spirit*= 1.04;
                    if (x == 2) stats.spirit*= 1.07;
                    if (x == 3) stats.spirit*= 1.1;
                }
                if (this.config.talents.arcane_mind)
                    stats.intellect*= 1.0 + this.config.talents.arcane_mind*0.03;
                if (this.config.race == this.races.RACE_GNOME)
                    stats.intellect*= 1.05;
                if (this.config.race == this.races.RACE_HUMAN)
                    stats.spirit*= 1.1;
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
                    stats.mana*= 1.02;

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
                    return "https://wotlkdb.com/?item="+id;
                if (this.item_source == "evo")
                    return "https://wotlk.evowow.com/?item="+id;
                return "https://wotlk.wowhead.com/?item="+id;
            },

            spellUrl(id) {
                if (typeof(id) == "object")
                    id = id.id;
                if (this.item_source == "wotlkdb")
                    return "https://wotlkdb.com/?spell="+id;
                if (this.item_source == "evo")
                    return "https://wotlk.evowow.com/?spell="+id;
                return "https://wotlk.wowhead.com/?spell="+id;
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

                if (slot.indexOf("trinket") === 0) {
                    var other = slot == "trinket1" ? "trinket2" : "trinket1";
                    if (this.isEquipped(other, item.id))
                        return;
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

                if (this.item_gems.hasOwnProperty(item.id)) {
                    this.gems[slot] = this.item_gems[item.id];
                }
                else {
                    this.gems[slot] = [null, null, null];
                    if (item.sockets) {
                        this.gems[slot] = this.defaultGems(item);
                        this.item_gems[item.id] = this.gems[slot];
                    }
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
                    if (gem.unique && this.isSocketedAnywhere(gem.id))
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

            isSocketedAnywhere(id) {
                for (var slot in this.gems) {
                    for (var i=0; i<this.gems[slot].length; i++) {
                        if (id == this.gems[slot][i])
                            return true;
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

            defaultGems(item) {
                var gems = [null, null, null];

                if (item.sockets) {
                    for (var i=0; i<item.sockets.length; i++)
                        gems[i] = this.defaultGem(item.sockets[i]);
                }

                return gems;
            },

            defaultGem(color) {
                if (color == "m")
                    return this.items.ids.META_CHAOTIC_SKYFLARE;
                return 1;
            },

            hasUseTrinket(nr) {
                var slot = "trinket"+nr;
                var item = this.equippedItem(slot);

                if (item && item.use)
                    return true;

                return false;
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
                if (index == -1) {
                    this.item_comparison.push({id: item.id, dps: null});
                }
                else {
                    this.item_comparison.splice(index, 1);
                }
            },

            comparisonDps(item) {
                var cmp = _.find(this.item_comparison, {id: item.id});
                var cmp2 = _.find(this.item_comparison, {id: this.items.ids.STAT_WEIGHT_BASE});
                if (cmp2 && cmp2.dps && item.id !== this.items.ids.STAT_WEIGHT_BASE)
                    return cmp && cmp.dps ? "+"+_.round(cmp.dps-cmp2.dps, 2) : null;
                return cmp && cmp.dps ? _.round(cmp.dps, 2) : null;
            },

            setSpec(spec) {
                if (spec == "arcane") {
                    this.config.build = "https://wotlk.evowow.com/?talent#of0Vfu0IzxGuMxebcZMhf0o:NzM0mM";
                    this.config.rotation = constants.rotations.ROTATION_ST_AB_AM;
                }
                else if (spec == "arcane_barrage") {
                    this.config.build = "https://wotlk.evowow.com/?talent#of0Vsu0IzxGuMxedcZMhf0o:NzM0mM";
                    this.config.rotation = constants.rotations.ROTATION_ST_AB_AM_BARRAGE;
                }
                else if (spec == "fire") {
                    this.config.build = "https://wotlk.evowow.com/?talent#of0Vck0cZ0Ec0RhIuVubhst:VLi0mc";
                    this.config.rotation = constants.rotations.ROTATION_ST_FIRE;
                }
                else if (spec == "frost") {
                    this.config.build = "https://wotlk.evowow.com/?talent#of0Vck0fZZVIccofuobzgfkt:Rqn0mc";
                    this.config.rotation = constants.rotations.ROTATION_ST_FROST;
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
                    else if (ch == "Z") {
                        talent = 0;
                        if (++tree == 3)
                            break;
                    }
                    else {
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
                }

                if (has_glyphs) {
                    var slot = 0;
                    var glyph, key;
                    var g = [[],[],[]];
                    var fnSort = function(a, b) { return a.id - b.id };
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

            round(num) {
                return Math.round(num);
            },

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
                var json = atob(this.import_profile.string);
                if (!json)
                    return;

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
                var json = atob(str);
                if (!json)
                    return this.importError("Could not parse import string");

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

            nukeSettings() {
                if (!window.confirm("This will remove all profiles and configurations from this computer"))
                    return;

                localStorage.clear();
                window.location.reload(true);
            },

            moveProfile(index, dir) {
                var pos = (this.profiles.length + index + dir) % this.profiles.length;
                this.profiles.splice(pos, 0, this.profiles.splice(index, 1)[0]);
                this.saveProfiles();
            },

            saveProfile(profile) {
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.gems = _.cloneDeep(this.gems);
                profile.config = _.cloneDeep(this.config);

                var index = _.findIndex(this.profiles, {id: profile.id});
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
                    _.merge(this.config, _.pick(profile.config, _.keys(this.config)));
                    this.onLoadConfig(profile.config);
                    this.profile_status.config = true;
                }

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
                this.custom_item.sockets = [null, null, null];
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
                if (item.slot != "weapon" || !item.twohand)
                    delete item.twohand;
                delete item.slot;

                if (!item.id)
                    item.id = this.createItemId();

                item.sockets = [];
                for (var i=0; i<this.custom_item.sockets; i++)
                    item.sockets.push("r");

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
                var index = _.findIndex(this.items.equip[this.active_slot], {id: item.id});
                if (index != -1) {
                    if (this.isEquipped(this.active_slot, item.id))
                        this.unequip(this.active_slot);
                    this.items.equip[this.active_slot].splice(index, 1);
                    this.saveCustomItems();
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
            },

            allResults() {
                var a = document.createElement("a");
                a.href = "data:text/csv,"+encodeURIComponent(this.result.all_results);
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
                this.parseTalents();
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
