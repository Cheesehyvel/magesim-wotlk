<template>
    <div id="app">
        <a class="github" href="https://github.com/Cheesehyvel/magesim-tbc2" target="_blank"></a>

        <div class="fools" v-if="fools_open == 1">
            <div class="inner">
                <div class="title">Buy premium</div>
                <div class="text mt-2">
                    You currently have <b>{{ fools_remaining }}</b> free sim(s) remaining!<br>
                    To get more sims sign up for our premium subscription of only <b>19.99€/month</b>!
                </div>
                <div class="btn large mt-2" @click="foolsBuy">Buy premium 19.99€</div>
                <div></div>
                <div class="btn-text mt-1" @click="foolsClose">No thanks, not right now</div>
            </div>
        </div>

        <div class="fools2" v-if="fools_open == 2">
            <div class="inner">
                <div class="title">Please buy premium</div>
                <div class="text mt-2">
                    Come on man! I've got dust bunnies to feed!<br>
                    For just 16.99€ I'll even toss in a free
                    <a href="https://www.ikea.com/se/sv/images/products/hjalpreda-osthyvel-svart__0392887_PE560396_S4.JPG" target="_blank">osthyvel</a>
                </div>
                <div class="btn mt-2" @click="foolsBuy">Buy premium 14.99€</div>
                <div></div>
                <div class="btn-text mt-1" @click="foolsClose">I don't care about you</div>
            </div>
        </div>

        <div class="fools2 notice" v-if="fools_open == 3">
            <div class="inner">
                <div class="title">Premium or else</div>
                <div class="text mt-2">
                    You know what? If you don't buy premium I'm just gonna mine Bitcoins in the background.
                </div>
                <div class="btn mt-2" @click="foolsBuy">Buy premium 9.99€</div>
                <div></div>
                <div class="btn-text mt-1" @click="foolsClose">Dude wtf, stop it</div>
            </div>
        </div>

        <div class="notice" v-if="donation_open" @click="donation_open = false">
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
                    <div class="btn block" @click="configToggle" :class="[is_running ? 'disabled' : '']">Config</div>
                    <div class="btn block mt-n" @click="runSingle" :class="[is_running ? 'disabled' : '']">Run</div>
                    <div class="btn block mt-n" @click="runMultiple" :class="[is_running ? 'disabled' : '']">Run {{ config.iterations }} times</div>
                    <div class="btn block mt-n" @click="runEP" :class="[is_running && !is_running_ep ? 'disabled' : '']">
                        <template v-if="!is_running_ep">Run stat weights</template>
                        <template v-else>Stop</template>
                    </div>
                </div>
                <div class="final-stats" v-if="final_stats">
                    <table class="simple">
                        <tbody>
                            <tr>
                                <td>Intellect</td>
                                <td>{{ final_stats.intellect }}</td>
                            </tr>
                            <tr>
                                <td>Spirit</td>
                                <td>{{ final_stats.spirit }}</td>
                            </tr>
                            <tr>
                                <td>Mp5</td>
                                <td>{{ final_stats.mp5 }}</td>
                            </tr>
                            <tr>
                                <td>Spell power</td>
                                <td>{{ final_stats.spell_power }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_arcane">
                                <td>SP Arcane</td>
                                <td>+{{ final_stats.spell_power_arcane }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_frost">
                                <td>SP Frost</td>
                                <td>+{{ final_stats.spell_power_frost }}</td>
                            </tr>
                            <tr v-if="final_stats.spell_power_fire">
                                <td>SP Fire</td>
                                <td>+{{ final_stats.spell_power_fire }}</td>
                            </tr>
                            <tr>
                                <td>Crit</td>
                                <td>
                                    <span>{{ $round(final_stats.crit, 2) }}%</span>
                                    <tooltip position="r">{{ final_stats.crit_rating }} crit rating</tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td>Hit</td>
                                <td>
                                    <span>{{ $round(final_stats.hit, 2) }}%</span>
                                    <tooltip position="r">{{ final_stats.hit_rating }} hit rating</tooltip>
                                </td>
                            </tr>
                            <tr>
                                <td>Haste</td>
                                <td>
                                    <span>{{ $round(final_stats.haste, 2) }}%</span>
                                    <tooltip position="r">{{ final_stats.haste_rating }} haste rating</tooltip>
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
                        <option value="sp_arcane">SP Arcane (EP)</option>
                        <option value="sp_frost">SP Frost (EP)</option>
                        <option value="sp_fire">SP Fire (EP)</option>
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
                            <tr @click="ep_weight = 'sp_arcane'">
                                <td>SP Arcane</td>
                                <td>{{ $nullRound(epCalc.sp_arcane, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp_frost'">
                                <td>SP Frost</td>
                                <td>{{ $nullRound(epCalc.sp_frost, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp_fire'">
                                <td>SP Fire</td>
                                <td>{{ $nullRound(epCalc.sp_fire, 2) }}</td>
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
                </div>
                <div class="result" v-else-if="result">
                    <template v-if="result.iterations">
                        <div>DPS</div>
                        <div>{{ $round(result.avg_dps, 2) }}</div>
                        <div class="faded">{{ $round(result.min_dps, 2) }} - {{ $round(result.max_dps, 2) }}</div>
                        <div class="mt-1"></div>
                        <div class="faded" v-if="result.stats.evocated.n">
                            Evocated: {{ $round(result.stats.evocated.t, 1) }}s
                            ({{ $round(result.stats.evocated.n / result.iterations * 100, 1) }}%)
                        </div>
                        <div class="faded" v-if="result.stats.regened.n">
                            Filler: {{ $round(result.stats.regened.t, 1) }}s
                            ({{ $round(result.stats.regened.n / result.iterations * 100, 1) }}%)
                        </div>
                        <div class="faded" v-if="result.stats.t_gcd_capped">
                            Wasted haste: {{ $round(result.stats.t_gcd_capped, 2) }}s
                            <help>Time spent gcd capped</help>
                        </div>
                        <div class="btn mt-1" v-if="result.histogram" @click="histogramToggle">Histogram</div>
                        <div class="btn mt-1" :class="[is_running ? 'disabled' : '']" @click="findAvg(result.avg_dps)">Find avg fight</div>
                        <div class="btn mt-1" v-if="result.all_results" @click="allResults">Simulation data</div>
                    </template>
                    <template v-else>
                        <div>DPS</div>
                        <div>{{ $round(result.dps, 2) }}</div>
                        <div>Damage: {{ result.dmg }}</div>
                        <div class="mt-1"></div>
                        <div class="faded" v-if="result.evocated_at > 0">Evocated at: {{ $round(result.evocated_at, 1) }}</div>
                        <div class="faded" v-if="result.regened_at > 0">Filler at: {{ $round(result.regened_at, 1) }}</div>
                        <div class="faded" v-if="result.t_gcd_capped">
                            Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s
                            <help>Time spent gcd capped</help>
                        </div>
                        <div class="btn mt-1" v-if="result.log" @click="logToggle">Combat log</div>
                        <div class="btn mt-1" v-if="result.log" @click="timelineToggle">Timeline</div>
                        <div class="btn mt-1" v-if="result.spells" @click="spellsToggle">Spells</div>
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
                <div class="donate">
                    <a href="https://www.paypal.com/donate/?hosted_button_id=CU9RF4LCMW8W6" target="_blank">
                        Donate
                    </a>
                </div>
            </div>
            <div class="main">
                <div class="gear">
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
                            <div class="top">
                                <div class="form-item">
                                    <select v-model="phase_filter">
                                        <option :value="0">- Filter by content phase -</option>
                                        <option :value="1">Phase 1 - KZ, Gruul, Mag, Arena S1</option>
                                        <option :value="2">Phase 2 - SSC, TK, Arena S2</option>
                                        <option :value="3">Phase 3 - MH, BT, Arena S3</option>
                                        <option :value="4">Phase 4 - Zul'Aman</option>
                                        <option :value="5">Phase 5 - SWP, Arena S4</option>
                                    </select>
                                </div>
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

                            <table class="mt-2">
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
                                        <th v-if="hasComparisons">
                                            {{ comparisonDps(item) }}
                                        </th>
                                        <th>{{ $get(item, "phase", 1) }}</th>
                                        <td>
                                            <template v-if="item.sockets">
                                                <div class="socket-color" :class="['color-'+socket]" v-for="socket in item.sockets"></div>
                                            </template>
                                            <span class="ml-n" v-if="item.bonus" :class="[hasSocketBonus(active_slot) ? 'socket-bonus' : '']">
                                                +{{ formatStats(item.bonus) }}
                                            </span>
                                        </td>
                                        <td>{{ formatSP(item) }}</td>
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

                            <table class="mt-4" v-if="activeEnchants.length">
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
                                        <td>{{ formatSP(item) }}</td>
                                        <td>{{ $get(item, "crit", "") }}</td>
                                        <td>{{ $get(item, "hit", "") }}</td>
                                        <td>{{ $get(item, "int", "") }}</td>
                                        <td>{{ $get(item, "spi", "") }}</td>
                                        <td>{{ $get(item, "mp5", "") }}</td>
                                    </tr>
                                </tbody>
                            </table>

                            <div class="sockets mt-4" v-if="activeSockets.length">
                                <div class="socket" v-for="(socket, index) in activeSockets">
                                    <div class="title">
                                        <span>Socket {{ (index+1) }}</span>
                                        <span class="socket-color" :class="['color-'+socket]"></span>
                                    </div>
                                    <table>
                                        <thead>
                                            <tr>
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

                <div class="log" v-if="log_open">
                    <div class="log-wrapper">
                        <div class="filter">
                            <div class="form-item">
                                <label><input type="checkbox" v-model="log_filter[2]"> <span>Show mana gain</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="log_filter[3]"> <span>Show buffs</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="log_filter[7]"> <span>Show GCD cap</span></label>
                            </div>
                        </div>
                        <table>
                            <thead>
                                <th>Time</th>
                                <th>Mana</th>
                                <th>DPS</th>
                                <th>Event</th>
                            </thead>
                            <tbody>
                                <tr
                                    v-for="log in result.log"
                                    v-if="showLog(log)"
                                    :class="['type-'+log.type]"
                                >
                                    <td>{{ formatTime(log.t) }}</td>
                                    <td>{{ round(log.mana) }} ({{ round(log.mana_percent) }}%)</td>
                                    <td>{{ (log.t ? round(log.dmg/log.t) : "0") }}</td>
                                    <td>{{ log.text }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    <div class="close" @click="logToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>

                <div class="timel" v-if="timeline_open">
                    <timeline ref="timeline" :result="result"></timeline>
                    <div class="close" @click="timelineToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>

                <div class="spells" v-if="spells_open">
                    <div class="spells-wrapper">
                        <table>
                            <thead>
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
                    <div class="close" @click="spellsToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>

                <div class="histog" v-if="histogram_open">
                    <histogram :data="result.histogram"></histogram>
                    <div class="close" @click="histogramToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>

                <div class="config" v-if="config_open">
                    <div class="fieldsets">
                        <fieldset>
                            <legend>General</legend>
                            <div class="form-item">
                                <span class="btn-text" @click="setSpec('arcane')">Arcane</span>
                                <span class="btn-text" @click="setSpec('fire')">Fire</span>
                                <span class="btn-text" @click="setSpec('frost')">Frost</span>
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
                                <label>
                                    <span>Shattrath Faction</span>
                                    <help>Controls the proc from Shattered Sun Pendant of Acumen necklace</help>
                                </label>
                                <select v-model="config.shatt_faction">
                                    <option :value="factions.FACTION_ALDOR">Aldor</option>
                                    <option :value="factions.FACTION_SCRYER">Scryer</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Talents (<a :href="config.talents" target="_blank">link</a>)</label>
                                <input type="text" :value="config.talents" @input="onTalentsInput">
                            </div>
                            <div class="form-item">
                                <label>Main spell</label>
                                <select v-model="config.main_rotation">
                                    <option :value="main_rotations.MAIN_ROTATION_AB">Arcane Blast</option>
                                    <option :value="main_rotations.MAIN_ROTATION_AE">Arcane Explosion</option>
                                    <option :value="main_rotations.MAIN_ROTATION_AM">Arcane Missiles</option>
                                    <option :value="main_rotations.MAIN_ROTATION_SC">Scorch</option>
                                    <option :value="main_rotations.MAIN_ROTATION_FIB">Fireball</option>
                                    <option :value="main_rotations.MAIN_ROTATION_FRB">Frostbolt</option>
                                </select>
                            </div>
                            <template v-if="config.main_rotation == main_rotations.MAIN_ROTATION_AB">
                                <div class="form-item">
                                    <label>Filler spells</label>
                                    <select v-model="config.regen_rotation">
                                        <option :value="regen_rotations.REGEN_ROTATION_FB">3xFrB</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_FB11">3xFrB (rank 11)</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_AMFB">1xAM, 1xFrB</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_AMSC">1xAM, 1xScorch</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_AMAM">2xAM</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_SC">5xScorch</option>
                                        <option :value="regen_rotations.REGEN_ROTATION_SCFB">1xScorch, 2xFiB</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Arcane Blasts between fillers</label>
                                    <select v-model="config.regen_ab_count">
                                        <option :value="1">1x AB</option>
                                        <option :value="2">2x AB</option>
                                        <option :value="3">3x AB</option>
                                        <option :value="4">4x AB</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Regen rotation at mana %</label>
                                    <input type="text" v-model.number="config.regen_mana_at">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Stop regen rotation at mana %</span>
                                        <help>Regen will always stop if it's possible to spam AB the rest of the fight</help>
                                    </label>
                                    <input type="text" v-model.number="config.regen_stop_at">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Stop Arcane Blast at haste %</span>
                                        <help>
                                            This will cast frostbolt/fireball/arcane missiles when above a certain haste %.<br>
                                            At 100% haste you will reach GCD cap of 1 second.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.ab_haste_stop">
                                </div>
                            </template>
                            <template v-if="canCream">
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.cc_am_queue">
                                        <span>Queue AM after clearcast</span>
                                        <help>
                                            Queue Arcane Missiles after a spell with clearcast active has been cast.<br>
                                            Arcane Missiles will gain the bonus from Arcane Potency but will still cost full mana.<br>
                                            This does not work if the cast time of the spell that consumes CC is shorter than the GCD.<br>
                                            Example: AB -> CC proc -> AB -> AM
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.cc_am_queue">
                                    <label>
                                        <input type="checkbox" v-model="config.cc_am_repeat">
                                        <span>Repeat AM if it procs CC</span>
                                    </label>
                                </div>
                            </template>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.fire_blast_weave"> <span>Fire Blast weave</span></label>
                            </div>
                            <div class="form-item">
                                <label>No. of targets</label>
                                <input type="text" v-model.number="config.targets">
                            </div>
                            <div class="form-item">
                                <label>Fight duration (sec)</label>
                                <input type="text" v-model.number="config.duration">
                            </div>
                            <div class="form-item">
                                <label>Duration +/- (sec)</label>
                                <input type="text" v-model.number="config.duration_variance">
                            </div>
                            <div class="form-item">
                                <label>Number of sims</label>
                                <input type="text" v-model.number="config.iterations">
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
                                <label><input type="checkbox" v-model="config.gcd_unlocked">
                                    <span>Unlock GCD</span>
                                    <help>Enables the GCD to go below 1.0s with haste</help>
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
                        </fieldset>
                        <fieldset>
                            <legend>Debuffs</legend>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.misery"> <span>Misery</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.curse_of_elements"> <span>Curse of Elements</span></label>
                            </div>
                            <div class="form-item" v-if="config.curse_of_elements">
                                <label><input type="checkbox" v-model="config.malediction">
                                    <span>Malediction</span>
                                    <help>3% extra damage from curse of elements</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.judgement_of_the_crusader"> <span>Imp. Judgement of the Crusader</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.judgement_of_wisdom"> <span>Judgement of Wisdom</span></label>
                            </div>
                            <div class="form-item" v-if="hasTalent('imp_scorch')">
                                <label><input type="checkbox" v-model="config.maintain_fire_vulnerability">
                                    <span>Keep up Fire Vulnerability</span>
                                    <help>Imp. Scorch from you</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.fire_vulnerability">
                                    <span>Fire Vulnerability</span>
                                    <help>Imp. Scorch from another mage</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.winters_chill">
                                    <span>Winter's Chill</span>
                                    <help>Winter's Chill from another mage</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.vampiric_touch"> <span>Vampiric Touch</span></label>
                            </div>
                            <div class="form-item" v-if="config.vampiric_touch">
                                <label>Vampiric Touch mana/sec</label>
                                <input type="text" v-model.number="config.vampiric_touch_regen">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Buffs</legend>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.arcane_intellect"> <span>Arcane Intellect</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mage_armor" @input="dontStack($event, 'molten_armor')"> <span>Mage Armor</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.molten_armor" @input="dontStack($event, 'mage_armor')"> <span>Molten Armor</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.divine_spirit" @input="dontStack($event, 'scroll_of_spirit')"> <span>Divine Spirit</span></label>
                            </div>
                            <div class="form-item" v-if="config.divine_spirit">
                                <label><input type="checkbox" v-model="config.improved_divine_spirit"> <span>Imp. Divine Spirit</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mark_of_the_wild"> <span>Mark of the Wild</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.totem_of_wrath"> <span>Totem of Wrath</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.wrath_of_air"> <span>Wrath of Air Totem</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.mana_spring"> <span>Mana Spring Totem</span></label>
                            </div>
                            <div class="form-item" v-if="config.mana_spring">
                                <label><input type="checkbox" v-model="config.improved_mana_spring"> <span>Imp. Mana Spring Totem</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.blessing_of_kings"> <span>Blessing of Kings</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.blessing_of_wisdom"> <span>Blessing of Wisdom</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.moonkin_aura"> <span>Moonkin Aura</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.imp_sanctity"> <span>Imp. Sanctity Aura</span></label>
                            </div>
                            <div class="form-item" v-if="faction == 'alliance'">
                                <label><input type="checkbox" v-model="config.inspiring_presence"> <span>Inspiring Presence (Draenei hit aura)</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.atiesh_mage">
                                    <span>Mage Atiesh Aura</span>
                                    <help>Another mage in your group has Atiesh</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.atiesh_warlock"> <span>Warlock Atiesh Aura</span></label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.eye_of_the_night" :disabled="isEquipped('neck', items.ids.EYE_OF_THE_NIGHT)">
                                    <span>Eye of the Night</span>
                                    <help>This is a party-wide buff from a JC necklace (34 sp)</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.chain_of_the_twilight_owl" :disabled="isEquipped('neck', items.ids.CHAIN_OF_THE_TWILIGHT_OWL)">
                                    <span>Chain of the Twilight Owl</span>
                                    <help>This is a party-wide buff from a JC necklace (2 crit)</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label><input type="checkbox" v-model="config.jade_pendant_of_blasting" :disabled="isEquipped('neck', items.ids.JADE_PENDANT_OF_BLASTING)">
                                    <span>Jade Pendant of Blasting</span>
                                    <help>This is a party-wide buff from a JC necklace (15 sp)</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label>Ferocious Inspirations</label>
                                <input type="text" v-model.number="config.ferocious_inspiration">
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Consumes</legend>
                            <div class="form-item" v-if="!config.battle_elixir && !config.guardian_elixir">
                                <label>Flask</label>
                                <select v-model="config.flask">
                                    <option :value="flasks.FLASK_NONE">None</option>
                                    <option :value="flasks.FLASK_SUPREME_POWER">Supreme Power (70 sp)</option>
                                    <option :value="flasks.FLASK_BLINDING_LIGHT">Blinding Light (80 arc)</option>
                                    <option :value="flasks.FLASK_PURE_DEATH">Pure Death (80 fire/frost)</option>
                                    <option :value="flasks.FLASK_DISTILLED_WISDOM">Distilled Wisdom (65 int)</option>
                                    <option :value="flasks.FLASK_CHROMATIC_WONDER">Chromatic Wonder (18 all stats)</option>
                                </select>
                            </div>
                            <div class="form-item" v-if="!config.flask">
                                <label>Battle Elixir</label>
                                <select v-model="config.battle_elixir">
                                    <option :value="elixirs.ELIXIR_NONE">None</option>
                                    <option :value="elixirs.ELIXIR_ADEPTS">Adept's Elixir (24 sp / 24 crit)</option>
                                    <option :value="elixirs.ELIXIR_GREATER_ARCANE">Greater Arcane (35 sp)</option>
                                    <option :value="elixirs.ELIXIR_MAJOR_FIREPOWER">Major Firepower (55 fire)</option>
                                    <option :value="elixirs.ELIXIR_MASTERY">Elixir of Mastery (15 all stats)</option>
                                </select>
                            </div>
                            <div class="form-item" v-if="!config.flask">
                                <label>Guardian Elixir</label>
                                <select v-model="config.guardian_elixir">
                                    <option :value="elixirs.ELIXIR_NONE">None</option>
                                    <option :value="elixirs.ELIXIR_DRAENIC_WISDOM">Draenic Wisdom (30 int / 30 spi)</option>
                                    <option :value="elixirs.ELIXIR_MAJOR_MAGEBLOOD">Major Mageblood (16 mp5)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Weapon oil</label>
                                <select v-model="config.weapon_oil">
                                    <option :value="weapon_oils.OIL_NONE">None</option>
                                    <option :value="weapon_oils.OIL_BRILLIANT_WIZARD">Brilliant Wizard Oil (36 sp / 14 crit)</option>
                                    <option :value="weapon_oils.OIL_SUPERIOR_WIZARD">Superior Wizard Oil (42 sp)</option>
                                    <option :value="weapon_oils.OIL_BLESSED_WIZARD">Blessed Wizard Oil (60 sp to undead)</option>
                                    <option :value="weapon_oils.OIL_SUPERIOR_MANA">Superior Mana Oil (14 mp5)</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Food</label>
                                <select v-model="config.food">
                                    <option :value="foods.FOOD_NONE">None</option>
                                    <option :value="foods.FOOD_SPELL_POWER">Blackened Basilisk (23 sp / 20 spi)</option>
                                    <option :value="foods.FOOD_SPELL_CRIT">Skullfish Soup (20 crit / 20 spi)</option>
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
                                    <help>Someone else in your party uses drums</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label>Potion</label>
                                <select v-model="config.potion">
                                    <option :value="potions.POTION_NONE">None</option>
                                    <option :value="potions.POTION_MANA">Mana potion</option>
                                    <option :value="potions.POTION_FEL_MANA">Fel Mana potion</option>
                                    <option :value="potions.POTION_DESTRUCTION">Destruction potion</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Conjured</label>
                                <select v-model="config.conjured">
                                    <option :value="conjureds.CONJURED_NONE">None</option>
                                    <option :value="conjureds.CONJURED_MANA_GEM">Mana Emerald</option>
                                    <option :value="conjureds.CONJURED_FLAME_CAP">Flame Cap</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>
                                    <input type="checkbox" v-model="config.scroll_of_spirit" @input="dontStack($event, ['divine_spirit', 'improved_divine_spirit'])">
                                    <span>Scroll of Spirit V</span>
                                    <help>Does not stack with Divine Spirit (30 spirit)</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label>
                                    <input type="checkbox" v-model="config.kreegs">
                                    <span>Kreeg's Stout Beatdown</span>
                                    <help>Stacks with other food buffs (25 spirit, -5 int)</help>
                                </label>
                            </div>
                            <div class="form-item" v-if="config.race == races.RACE_BLOOD_ELF">
                                <label>
                                    <input type="checkbox" v-model="config.bloodthistle">
                                    <span>Bloodthistle</span>
                                    <help>10sp buff, only usable by Blood Elfs</help>
                                </label>
                            </div>
                            <div class="form-item">
                                <label>
                                    <input type="checkbox" v-model="config.scourgebane">
                                    <span>Scourgebane Infusion</span>
                                    <help>15sp buff against undead</help>
                                </label>
                            </div>
                        </fieldset>
                        <fieldset>
                            <legend>Cooldowns</legend>
                            <template v-if="hasTalent('presence_of_mind')">
                                <div class="form-item">
                                    <label>
                                        <span>Presence of Mind timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.presence_of_mind_t">
                                        <input type="text" v-model.number="config.presence_of_mind_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasTalent('arcane_power')">
                                <div class="form-item">
                                    <label>
                                        <span>Arcane Power timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.arcane_power_t">
                                        <input type="text" v-model.number="config.arcane_power_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasTalent('icy_veins')">
                                <div class="form-item">
                                    <label>
                                        <span>Icy Veins timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.icy_veins_t">
                                        <input type="text" v-model.number="config.icy_veins_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasTalent('cold_snap')">
                                <div class="form-item">
                                    <label>
                                        <span>Cold Snap timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.cold_snap_t">
                                        <input type="text" v-model.number="config.cold_snap_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasTalent('combustion')">
                                <div class="form-item">
                                    <label>
                                        <span>Combustion timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.combustion_t">
                                        <input type="text" v-model.number="config.combustion_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="config.race == races.RACE_TROLL">
                                <div class="form-item">
                                    <label>
                                        <span>Berserking timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.berserking_t">
                                        <input type="text" v-model.number="config.berserking_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="config.potion && config.potion != potions.POTION_MANA && config.potion != potions.POTION_FEL_MANA">
                                <div class="form-item">
                                    <label>
                                        <span>Potion timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.potion_t">
                                        <input type="text" v-model.number="config.potion_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="config.conjured">
                                <div class="form-item">
                                    <label>
                                        <span>Conjured timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.conjured_t">
                                        <input type="text" v-model.number="config.conjured_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasUseTrinket(1)">
                                <div class="form-item">
                                    <label>
                                        <span>Trinket #1 timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.trinket1_t">
                                        <input type="text" v-model.number="config.trinket1_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="hasUseTrinket(2)">
                                <div class="form-item">
                                    <label>
                                        <span>Trinket #2 timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.trinket2_t">
                                        <input type="text" v-model.number="config.trinket2_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template v-if="config.drums">
                                <div class="form-item">
                                    <label>
                                        <span>Drums timings</span>
                                        <timing-helper></timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.drums_t">
                                        <input type="text" v-model.number="config.drums_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template>
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.bloodlust">
                                        <span>
                                            Bloodlust
                                            <span v-if="config.bloodlust">
                                                timings
                                                <timing-helper :nocd="true"></timing-helper>
                                            </span>
                                        </span>
                                    </label>
                                </div>
                                <div class="form-row mt-0" v-if="config.bloodlust">
                                    <div class="form-item" v-for="(a, i) in config.bloodlust_t">
                                        <input type="text" v-model.number="config.bloodlust_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template>
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.power_infusion">
                                        <span>
                                            Power Infusion
                                            <span v-if="config.power_infusion">
                                                timings
                                                <timing-helper :nocd="true">Does not stack with Arcane Power</timing-helper>
                                            </span>
                                        </span>
                                    </label>
                                </div>
                                <div class="form-row mt-0" v-if="config.power_infusion">
                                    <div class="form-item" v-for="(a, i) in config.power_infusion_t">
                                        <input type="text" v-model.number="config.power_infusion_t[i]">
                                    </div>
                                </div>
                            </template>
                            <template>
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.mana_tide">
                                        <span>
                                            Mana Tide
                                            <template v-if="config.mana_tide">
                                                timings
                                                <timing-helper :nocd="true"></timing-helper>
                                            </template>
                                        </span>
                                    </label>
                                </div>
                                <div class="form-row mt-0" v-if="config.mana_tide">
                                    <div class="form-item" v-for="(a, i) in config.mana_tide_t">
                                        <input type="text" v-model.number="config.mana_tide_t[i]">
                                    </div>
                                </div>
                            </template>
                            <div class="form-item" v-if="faction == 'alliance'">
                                <label><input type="checkbox" v-model="config.symbol_of_hope">
                                    <span>Symbol of Hope <template v-if="config.symbol_of_hope">at</template></span>
                                    <help>Draenei priest racial.<br>Setting this to 0 will automatically cast it when mana is low.</help>
                                </label>
                                <input type="text" v-model.number="config.symbol_of_hope_at" v-if="config.symbol_of_hope">
                            </div>
                            <div class="form-row">
                                <div class="form-item">
                                    <label>
                                        <span>Evocation at</span>
                                        <help>Setting this to 0 will evocate when mana is low</help>
                                    </label>
                                    <input type="text" v-model.number="config.evocation_at">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Cancel after n ticks</span>
                                        <help>Setting this to 0 will not cancel evocation.</help>
                                    </label>
                                    <input type="text" v-model.number="config.evo_ticks">
                                </div>
                            </div>
                            <div class="form-item">
                                <label>Number of innervates</label>
                                <input type="text" v-model.number="config.innervate">
                            </div>
                            <template v-if="config.innervate > 0">
                                <div class="form-item">
                                    <label>
                                        <span>Innervate timings</span>
                                        <timing-helper :nocd="true">Leaving empty will innervate when mana is low.</timing-helper>
                                    </label>
                                </div>
                                <div class="form-row mt-0">
                                    <div class="form-item" v-for="(a, i) in config.innervate_t" v-if="i < config.innervate">
                                        <input type="text" v-model.number="config.innervate_t[i]">
                                    </div>
                                </div>
                            </template>
                        </fieldset>
                        <fieldset class="profiles-fieldset">
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
                                <div class="export-import mt-1">
                                    <div class="btn" @click="openExport()">Export</div>
                                    <div class="btn ml-n" @click="openImport()">Import</div>
                                </div>
                            </div>
                        </fieldset>
                    </div>
                    <div class="close" @click="configToggle">
                        <span class="material-icons">
                            &#xe5cd;
                        </span>
                    </div>
                </div>
            </div>

            <div class="lightbox" v-if="export_profile.open">
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
                            <label>Spell Power (fire)</label>
                            <input type="number" v-model.number="custom_item.sp_fire">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power (frost)</label>
                            <input type="number" v-model.number="custom_item.sp_frost">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power (arcane)</label>
                            <input type="number" v-model.number="custom_item.sp_arcane">
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
    import constants from "./constants";

    export default {
        mounted() {
            this.loadCustomItems();
            this.loadConfig();
            this.loadGear();
            this.loadProfiles();
            this.finalStats();

            this.checkDonation();
        },

        data() {
            var default_config = {
                iterations: 30000,
                race: 5,
                shatt_faction: 0,

                duration: 180,
                duration_variance: 0,
                rng_seed: 0,
                gcd_unlocked: false,
                avg_spell_dmg: false,
                additional_data: false,
                targets: 1,

                misery: true,
                curse_of_elements: true,
                malediction: false,
                judgement_of_the_crusader: false,
                judgement_of_wisdom: true,
                vampiric_touch: true,
                vampiric_touch_regen: 40,

                totem_of_wrath: true,
                wrath_of_air: true,
                mana_spring: true,
                improved_mana_spring: false,
                arcane_intellect: true,
                divine_spirit: true,
                improved_divine_spirit: false,
                blessing_of_kings: true,
                blessing_of_wisdom: true,
                mark_of_the_wild: true,
                moonkin_aura: false,
                mage_armor: true,
                molten_armor: false,
                inspiring_presence: false,
                fire_vulnerability: false,
                maintain_fire_vulnerability: false,
                winters_chill: false,
                imp_sanctity: false,
                ferocious_inspiration: 0,

                food: 0,
                flask: 0,
                battle_elixir: 0,
                guardian_elixir: 0,
                weapon_oil: 0,
                drums: 0,
                drums_friend: false,
                potion: 22832,
                conjured: 22044,
                atiesh_mage: false,
                atiesh_warlock: false,
                eye_of_the_night: false,
                chain_of_the_twilight_owl: false,
                jade_pendant_of_blasting: false,
                scroll_of_spirit: false,
                kreegs: false,
                bloodthistle: false,
                scourgebane: false,

                tirisfal_2set: true,
                tirisfal_4set: true,
                tempest_2set: false,
                tempest_4set: false,
                spellfire_set: false,
                spellstrike_set: false,
                eternal_sage: false,
                wrath_of_cenarius: false,
                blade_of_wizardry: false,
                robe_elder_scribes: false,
                blade_of_eternal_darkness: false,
                sunwell_neck_aldor: false,
                sunwell_neck_scryer: false,
                mana_etched_4set: false,
                meta_gem: 0,
                trinket1: 0,
                trinket2: 0,

                innervate: 0,
                mana_tide: true,
                bloodlust: true,
                power_infusion: false,
                symbol_of_hope: false,

                main_rotation: 0,
                regen_rotation: 0,
                regen_mana_at: 20,
                regen_stop_at: 30,
                regen_ab_count: 3,
                ab_haste_stop: 0,
                fire_blast_weave: false,

                cc_am_queue: false,
                cc_am_repeat: false,

                bis_ashtongue: false,

                trinket1_t: Array(4),
                trinket2_t: Array(4),
                arcane_power_t: Array(4),
                presence_of_mind_t: Array(4),
                icy_veins_t: Array(4),
                cold_snap_t: Array(4),
                combustion_t: Array(4),
                berserking_t: Array(4),
                mana_tide_t: Array(4),
                bloodlust_t: Array(4),
                power_infusion_t: Array(4),
                drums_t: Array(4),
                innervate_t: Array(4),
                potion_t: Array(4),
                conjured_t: Array(4),

                symbol_of_hope_at: 0,
                evocation_at: 0,
                evo_ticks: 0,

                talents: "https://tbc.wowhead.com/talent-calc/mage/2500250300030150330125--053500031003001",

                stats: {
                    intellect: 465,
                    spirit: 285,
                    mp5: 0,
                    crit: 20,
                    hit: 0,
                    haste: 0,
                    spell_power: 1000,
                    spell_power_arcane: 50,
                    spell_power_frost: 0,
                    spell_power_fire: 0,
                    crit_rating: 0,
                    hit_rating: 0,
                    haste_rating: 0,
                },

                tooltips: false,
            };

            var data = {
                ...constants,
                fools_open: 0,
                fools_remaining: 3,
                donation_open: false,
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
                    sp_fire: null,
                    sp_arcane: null,
                    sp_frost: null,
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
                final_stats: null,
                result: null,
                ep_result: null,
                ep_weight: "dps",
                is_running: false,
                is_running_ep: false,
                config_open: false,
                log_open: false,
                timeline_open: false,
                spells_open: false,
                histogram_open: false,
                item_source: "wowhead",
                phase_filter: 0,
                log_filter: {
                    "0": true,
                    "1": true,
                    "2": false,
                    "3": true,
                    "4": true,
                    "5": true,
                    "6": true,
                    "7": true,
                },
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

            return data;
        },

        computed: {
            foolsActive() {
                var d = new Date;
                return d.getMonth() == 3 && d.getDate() == 1;
            },

            spec() {
                if (this.config.main_rotation <= this.main_rotations.MAIN_ROTATION_AM)
                    return "arcane";
                if (this.config.main_rotation <= this.main_rotations.MAIN_ROTATION_FIB)
                    return "fire";
                if (this.config.main_rotation <= this.main_rotations.MAIN_ROTATION_FRB)
                    return "frost";
                return null;
            },

            canCream() {
                return this.hasTalent('clearcast') &&
                    this.config.main_rotation != this.main_rotations.MAIN_ROTATION_AM &&
                    this.config.main_rotation != this.main_rotations.MAIN_ROTATION_AE;
            },

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
                return item && item.sockets ? item.sockets : [];
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
                    sp_arcane: null,
                    sp_frost: null,
                    sp_fire: null,
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
            }
        },

        methods: {
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

                    if (sorting.name == "sp") {
                        av = Math.max(_.get(a, "sp", 0), _.get(a, "sp_fire", 0), _.get(a, "sp_frost", 0), _.get(a, "sp_arcane", 0));
                        bv = Math.max(_.get(b, "sp", 0), _.get(b, "sp_fire", 0), _.get(b, "sp_frost", 0), _.get(b, "sp_arcane", 0));
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

            foolsBuy() {
                window.location.href = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";
            },

            foolsClose() {
                this.fools_open = (this.fools_open+1)%4;
            },

            foolsOpen() {
                if (!this.foolsActive || !this.fools_remaining)
                    return;
                this.fools_remaining = Math.max(this.fools_remaining-1, 0);
                this.fools_open = 1;
            },

            checkDonation() {
                if (window.location.hash == "#donation") {
                    window.location.hash = "";
                    this.donation_open = true;
                }
            },

            runMultiple() {
                var self = this;
                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                    self.foolsOpen();
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                this.timeline_open = false;
                this.spells_open = false;
                this.log_open = false;
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
                    self.foolsOpen();
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                this.histogram_open = false;
                this.ep_result = null;
                this.prepare();
                this.is_running = true;
                sim.start(this.config);
            },

            async findAvg(avg) {
                this.histogram_open = false;
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
                        sp_arcane: 0,
                        sp_frost: 0,
                        sp_fire: 0,
                    }, stats);

                    config.stats.intellect+= stats.int;
                    config.stats.spirit+= stats.spi;
                    config.stats.mp5+= stats.mp5;
                    config.stats.spell_power+= stats.sp;
                    config.stats.spell_power_arcane+= stats.sp_arcane;
                    config.stats.spell_power_frost+= stats.sp_frost;
                    config.stats.spell_power_fire+= stats.sp_fire;
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

                    self.log_open = false;
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

                this.is_running_ep = true;
                this.result = null;
                this.ep_result = {
                    base: null,
                    int: null,
                    spi: null,
                    mp5: null,
                    sp: null,
                    sp_arcane: null,
                    sp_frost: null,
                    sp_fire: null,
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

                this.foolsOpen();
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

                    self.log_open = false;
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

                this.foolsOpen();
            },

            prepare() {
                this.fillEmptyFields();
                this.saveGear();
                this.saveConfig();
                this.itemStats();
                this.itemConfig();
                this.finalStats();
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

            activeGems(index) {
                if (this.activeSockets.length < index)
                    return [];
                if (this.activeSockets[index] == "m")
                    var gems = this.items.gems.filter(g => g.color == "m");
                else
                    var gems = this.items.gems.filter(g => g.color != "m");

                if (!this.phase_filter)
                    return gems;

                return gems.filter(g => _.get(g, "phase", 1) <= this.phase_filter);
            },

            fillEmptyFields() {
                for (var key in this.config) {
                    if (this.config[key] === "" || this.config[key] === null)
                        this.config[key] = this.default_config.hasOwnProperty(key) ? this.default_config[key] : 0;
                }
            },

            finalStats() {
                var x;
                this.itemStats();
                this.itemConfig();

                var stats = _.cloneDeep(this.config.stats);

                // Attribute additions
                if (this.config.arcane_intellect)
                    stats.intellect+= 40;
                if (this.config.divine_spirit)
                    stats.spirit+= 40;
                if (this.config.guardian_elixir == this.elixirs.ELIXIR_DRAENIC_WISDOM) {
                    stats.intellect+= 30;
                    stats.spirit+= 30;
                }
                if (this.config.battle_elixir == this.elixirs.ELIXIR_MASTERY) {
                    stats.intellect+= 15;
                    stats.spirit+= 15;
                }
                if (this.config.mark_of_the_wild) {
                    stats.intellect+= 18;
                    stats.spirit+= 18;
                }
                if (this.config.flask == this.flasks.FLASK_DISTILLED_WISDOM)
                    stats.intellect+= 65;
                if (this.config.flask == this.flasks.FLASK_CHROMATIC_WONDER) {
                    stats.intellect+= 18;
                    stats.spirit+= 18;
                }
                if (this.config.food == this.foods.FOOD_SPELL_POWER || this.config.food == this.foods.FOOD_SPELL_CRIT)
                    stats.spirit+= 20;
                if (this.config.scroll_of_spirit)
                    stats.spirit+= 30;
                if (this.config.kreegs) {
                    stats.spirit+= 25;
                    stats.intellect-= 5;
                }

                // Attribute multipliers
                if (x = this.hasTalent("arcane_mind"))
                    stats.intellect*= 1.0 + x*0.03;
                if (this.config.race == this.races.RACE_GNOME)
                    stats.intellect*= 1.05;
                if (this.config.race == this.races.RACE_HUMAN)
                    stats.spirit*= 1.1;
                if (this.config.blessing_of_kings) {
                    stats.intellect*= 1.1;
                    stats.spirit*= 1.1;
                }
                if (this.metaGem() && this.metaGem().id == this.items.ids.EMBER_SKYFIRE)
                    stats.intellect*= 1.02;
                stats.intellect = Math.round(stats.intellect);
                stats.spirit = Math.round(stats.spirit);

                // Mp5
                if (this.config.guardian_elixir == this.elixirs.ELIXIR_MAJOR_MAGEBLOOD)
                    stats.mp5+= 16;
                if (this.config.weapon_oil == this.weapon_oils.OIL_SUPERIOR_MANA)
                    stats.mp5+= 14;
                if (this.config.blessing_of_wisdom)
                    stats.mp5+= 49;

                // Spell power
                var int_multi = 0;
                if (x = this.hasTalent("mind_mastery"))
                    int_multi+= x*0.05;
                if (this.config.spellfire_set)
                    int_multi+= 0.07;
                if (int_multi > 0)
                    stats.spell_power+= Math.round(stats.intellect * int_multi);

                if (this.config.improved_divine_spirit && this.config.divine_spirit)
                    stats.spell_power+= stats.spirit*0.1;
                if (this.config.wrath_of_air)
                    stats.spell_power+= 101;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_WIZARD)
                    stats.spell_power+= 36;
                if (this.config.weapon_oil == this.weapon_oils.OIL_SUPERIOR_WIZARD)
                    stats.spell_power+= 42;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BLESSED_WIZARD)
                    stats.spell_power+= 60;
                if (this.config.food == this.foods.FOOD_SPELL_POWER)
                    stats.spell_power+= 23;
                if (this.config.flask == this.flasks.FLASK_SUPREME_POWER)
                    stats.spell_power+= 70;
                if (this.config.flask == this.flasks.FLASK_BLINDING_LIGHT)
                    stats.spell_power_arcane+= 80;
                if (this.config.flask == this.flasks.FLASK_PURE_DEATH) {
                    stats.spell_power_fire+= 80;
                    stats.spell_power_frost+= 80;
                }
                if (this.config.battle_elixir == this.elixirs.ELIXIR_ADEPTS)
                    stats.spell_power+= 24;
                if (this.config.battle_elixir == this.elixirs.ELIXIR_GREATER_ARCANE)
                    stats.spell_power+= 35;
                if (this.config.battle_elixir == this.elixirs.ELIXIR_MAJOR_FIREPOWER)
                    stats.spell_power_fire+= 55;
                if (this.config.atiesh_warlock)
                    stats.spell_power+= 33;
                if (this.config.eye_of_the_night)
                    stats.spell_power+= 34;
                if (this.config.jade_pendant_of_blasting)
                    stats.spell_power+= 15;
                if (this.config.bloodthistle && this.config.race == this.races.RACE_BLOOD_ELF)
                    stats.spell_power+= 10;
                if (this.config.scourgebane)
                    stats.spell_power+= 15;

                // Spell crit
                var crit_rating = 0;
                if (this.config.judgement_of_the_crusader)
                    stats.crit+= 3;
                if (this.config.moonkin_aura)
                    stats.crit+= 5;
                if (this.config.totem_of_wrath)
                    stats.crit+= 3;
                if (this.config.molten_armor)
                    stats.crit+= 3;
                if (this.config.chain_of_the_twilight_owl)
                    stats.crit+= 2;
                if (this.config.battle_elixir == this.elixirs.ELIXIR_ADEPTS)
                    crit_rating+= 24;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_WIZARD)
                    crit_rating+= 14;
                if (this.config.food == this.foods.FOOD_SPELL_CRIT)
                    crit_rating+= 20;
                if (this.config.atiesh_mage)
                    crit_rating+= 28;
                if (x = this.hasTalent("arcane_instability"))
                    stats.crit+= x;
                if (crit_rating > 0) {
                    stats.crit+= this.critRatingToChance(crit_rating);
                    stats.crit_rating+= crit_rating;
                }
                stats.crit+= stats.intellect/80;

                // Spell hit
                if (this.config.totem_of_wrath)
                    stats.hit+= 3;
                if (this.config.race == this.races.RACE_DRAENEI || this.faction == "alliance" && this.config.inspiring_presence)
                    stats.hit+= 1;

                this.final_stats = stats;
            },

            baseStats() {
                // Undead default
                var stats = {
                    intellect: 149,
                    spirit: 150,
                    mp5: 0,
                    crit: 0.91,
                    hit: 0,
                    haste: 0,
                    spell_power: 0,
                    spell_power_arcane: 0,
                    spell_power_frost: 0,
                    spell_power_fire: 0,
                    crit_rating: 0,
                    hit_rating: 0,
                    haste_rating: 0,
                };

                if (this.config.race == this.races.RACE_TROLL) {
                    stats.intellect = 147;
                    stats.spirit = 146;
                }
                if (this.config.race == this.races.RACE_BLOOD_ELF) {
                    stats.intellect = 155;
                    stats.spirit = 144;
                }
                if (this.config.race == this.races.RACE_DRAENEI) {
                    stats.intellect = 152;
                    stats.spirit = 147;
                }
                if (this.config.race == this.races.RACE_GNOME) {
                    stats.intellect = 155;
                    stats.spirit = 145;
                }
                if (this.config.race == this.races.RACE_HUMAN) {
                    stats.intellect = 151;
                    stats.spirit = 145;
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
                    sp_arcane: 0,
                    sp_frost: 0,
                    sp_fire: 0,
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

                stats.intellect+= item_stats.int;
                stats.spirit+= item_stats.spi;
                stats.mp5+= item_stats.mp5;
                stats.spell_power+= item_stats.sp;
                stats.spell_power_arcane+= item_stats.sp_arcane;
                stats.spell_power_frost+= item_stats.sp_frost;
                stats.spell_power_fire+= item_stats.sp_fire;
                stats.crit+= this.critRatingToChance(item_stats.crit);
                stats.hit+= this.hitRatingToChance(item_stats.hit);
                stats.haste+= this.hasteRatingToHaste(item_stats.haste);

                stats.crit_rating = item_stats.crit;
                stats.hit_rating = item_stats.hit;
                stats.haste_rating = item_stats.haste;

                stats.crit = stats.crit;
                stats.hit = stats.hit;
                stats.haste = stats.haste;

                this.config.stats = stats;
            },

            itemConfig() {
                var num = this.numEquippedSet(this.items.ids.TIRISFAL_SET);
                this.config.tirisfal_2set = num > 1;
                this.config.tirisfal_4set = num > 3;

                num = this.numEquippedSet(this.items.ids.TEMPEST_SET);
                this.config.tempest_2set = num > 1;
                this.config.tempest_4set = num > 3;

                this.config.spellstrike_set = this.numEquippedSet(this.items.ids.SPELLSTRIKE_SET) > 1;
                this.config.spellfire_set = this.numEquippedSet(this.items.ids.SPELLFIRE_SET) > 2;

                this.config.mana_etched_4set = this.numEquippedSet(this.items.ids.MANA_ETCHED_SET) > 3;

                this.config.eternal_sage = this.isEquipped("finger", this.items.ids.ETERNAL_SAGE);
                this.config.wrath_of_cenarius = this.isEquipped("finger", this.items.ids.WRATH_OF_CENARIUS);
                this.config.blade_of_wizardry = this.isEquipped("weapon", this.items.ids.BLADE_OF_WIZARDRY);
                this.config.blade_of_eternal_darkness = this.isEquipped("weapon", this.items.ids.BLADE_OF_ETERNAL_DARKNESS);
                this.config.robe_elder_scribes = this.isEquipped("chest", this.items.ids.ROBE_ELDER_SCRIBES);

                this.config.sunwell_neck_aldor = (this.isEquipped("neck", this.items.ids.PENDANT_OF_ACUMEN) && this.config.shatt_faction == this.factions.FACTION_ALDOR);
                this.config.sunwell_neck_scryer = (this.isEquipped("neck", this.items.ids.PENDANT_OF_ACUMEN) && this.config.shatt_faction == this.factions.FACTION_SCRYER);

                if (this.isEquipped("neck", this.items.ids.EYE_OF_THE_NIGHT))
                    this.config.eye_of_the_night = true;
                if (this.isEquipped("neck", this.items.ids.CHAIN_OF_THE_TWILIGHT_OWL))
                    this.config.chain_of_the_twilight_owl = true;
                if (this.isEquipped("neck", this.items.ids.JADE_PENDANT_OF_BLASTING))
                    this.config.jade_pendant_of_blasting = true;

                this.config.trinket1 = 0;
                this.config.trinket2 = 0;
                this.config.meta_gem = 0;
                if (this.isSpecialItem(this.equipped.trinket1))
                    this.config.trinket1 = this.equipped.trinket1;
                if (this.isSpecialItem(this.equipped.trinket2))
                    this.config.trinket2 = this.equipped.trinket2;
                if (this.metaGem() && this.isSpecialItem(this.metaGem().id) && this.isMetaGemActive())
                    this.config.meta_gem = this.metaGem().id;
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
                if (this.item_source == "tbcdb")
                    return "https://tbcdb.com/?item="+id;
                if (this.item_source == "endless")
                    return "https://db.endless.gg/?item="+id;
                if (this.item_source == "twinstar")
                    return "https://tbc-twinhead.twinstar.cz/?item="+id;
                return "https://tbc.wowhead.com/?item="+id;
            },

            spellUrl(id) {
                if (typeof(id) == "object")
                    id = id.id;
                if (this.item_source == "tbcdb")
                    return "https://tbcdb.com/?spell="+id;
                if (this.item_source == "endless")
                    return "https://db.endless.gg/?spell="+id;
                if (this.item_source == "twinstar")
                    return "https://tbc-twinhead.twinstar.cz/?spell="+id;
                return "https://tbc.wowhead.com/?spell="+id;
            },

            critRatingToChance(rating) {
                return rating / 22.08;
            },

            hitRatingToChance(rating) {
                return rating / 12.62;
            },

            hasteRatingToHaste(rating) {
                return rating / 15.77;
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
                    this.saveConfig();
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

                this.finalStats();
                if (typeof(save) == "undefined" || save)
                    this.saveGear();
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

                this.finalStats();
                if (typeof(save) == "undefined" || save)
                    this.saveGear();

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

            enchant(slot, item) {
                if (this.enchants[slot] == item.id)
                    this.enchants[slot] = null;
                else
                    this.enchants[slot] = item.id;

                this.saveGear();
                this.finalStats();
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

                this.saveGear();
                this.finalStats();
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

                this.saveGear();
                this.finalStats();
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
                    return this.items.ids.CHAOTIC_SKYFIRE;

                if (this.config.main_rotation == this.main_rotations.MAIN_ROTATION_AB)
                    return this.items.ids.BRILLIANT_DAWNSTONE;
                return this.items.ids.RUNED_LIVING_RUBY;
            },

            setSpec(spec) {
                var talents = null;

                if (spec == "arcane") {
                    talents = "https://tbc.wowhead.com/talent-calc/mage/2500250300030150330125--053500031003001";
                    if (this.spec != "arcane")
                        this.config.main_rotation = this.main_rotations.MAIN_ROTATION_AB;
                }
                else if (spec == "frost") {
                    talents = "https://tbc.wowhead.com/talent-calc/mage/2500250300030150330125--053500031003001";
                    if (this.spec != "frost")
                        this.config.main_rotation = this.main_rotations.MAIN_ROTATION_FRB;
                }
                else if (spec == "fire") {
                    talents = "https://tbc.wowhead.com/talent-calc/mage/2-505202012303331053125-043500001";
                    if (this.spec != "fire")
                        this.config.main_rotation = this.main_rotations.MAIN_ROTATION_FIB;
                }

                if (talents)
                    this.config.talents = talents;
            },

            hasTalent(talent) {
                var indexes = {
                    clearcast: [0, 5],
                    presence_of_mind: [0, 13],
                    arcane_mind: [0, 14],
                    arcane_instability: [0, 16],
                    arcane_power: [0, 19],
                    mind_mastery: [0, 21],
                    imp_scorch: [1, 9],
                    combustion: [1, 18],
                    icy_veins: [2, 8],
                    cold_snap: [2, 14],
                };

                if (!indexes.hasOwnProperty(talent))
                    return false;

                var m = this.config.talents.match(/tbc\.wowhead\.com.*mage\/([0-9\-]+)$/);
                if (m) {
                    var str = m[1];
                    var tree = 0, t = 0;

                    for (var i=0; i<str.length; i++) {
                        if (str[i] == '-') {
                            tree++;
                            t = 0;
                        }
                        else {
                            if (tree == indexes[talent][0] && t == indexes[talent][1])
                                return parseInt(str[i]);
                            t++;
                        }
                    }
                }

                return false;
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

            onTalentsInput(e) {
                this.config.talents = this.conformTalents(e.target.value);
                e.preventDefault();
            },

            conformTalents(talents) {
                var m;

                if (m = talents.match(/tbcdb\.com.*mage\&([0-9]+)/i)) {
                    var fn = function(str) { return str.replace(/[0]+$/, ""); }
                    talents = "https://tbc.wowhead.com/talent-calc/mage/"+fn(m[1].substr(0, 23))+"-"+fn(m[1].substr(23, 22))+"-"+fn(m[1].substr(45));
                }

                return talents;
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

            formatSP(data) {
                var arr = [];

                if (data.sp)
                    arr.push(data.sp);
                if (data.sp_arcane)
                    arr.push(data.sp_arcane+" arc");
                if (data.sp_frost)
                    arr.push(data.sp_frost+" frost");
                if (data.sp_fire)
                    arr.push(data.sp_fire+" fire");

                if (data.spell_power)
                    arr.push(data.spell_power);
                if (data.spell_power_arcane)
                    arr.push(data.spell_power_arcane+" arc");
                if (data.spell_power_frost)
                    arr.push(data.spell_power_frost+" frost");
                if (data.spell_power_fire)
                    arr.push(data.spell_power_fire+" fire");

                return arr.join(" / ");
            },

            formatTime(s) {
                var dec = Math.round((s%1) * 100);
                var sec = Math.floor(s);
                if (dec == 100) {
                    dec = 0;
                    sec++;
                }

                return (sec < 10 ? "0"+sec : sec)+"."+(dec < 10 ? "0"+dec : dec);
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

            moveProfile(index, dir) {
                var pos = (this.profiles.length + index + dir) % this.profiles.length;
                this.profiles.splice(pos, 0, this.profiles.splice(index, 1)[0]);
                this.saveProfiles();
            },

            saveProfile(profile) {
                profile.equipped = _.cloneDeep(this.equipped);
                delete profile.equipped.stat_weight;
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
                    profile.enchants = this.convertEnchants(profile.enchants);
                    for (var slot in profile.enchants) {
                        if (!this.getEnchant(slot, profile.enchants[slot]))
                            profile.enchants[slot] = null;
                    }
                    _.merge(this.enchants, profile.enchants);
                }

                if (profile.gems && (!only || only == "items")) {
                    profile.gems = _.pick(profile.gems, _.keys(this.gems));
                    profile.gems = this.convertGems(profile.gems);
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

                this.finalStats();

                if (!only || only == "items")
                    this.saveGear();

                if (profile.config && (!only || only == "config"))
                    this.saveConfig();

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
                this.custom_item.sp_fire = null;
                this.custom_item.sp_frost = null;
                this.custom_item.sp_arcane = null;
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

            showLog(log) {
                return this.log_filter[log.type];
            },

            configToggle() {
                this.timeline_open = false;
                this.spells_open = false;
                this.histogram_open = false;
                this.log_open = false;
                this.config_open = !this.config_open;
                if (!this.config_open) {
                    this.saveConfig();
                    this.finalStats();
                }
            },

            logToggle() {
                this.timeline_open = false;
                this.spells_open = false;
                this.histogram_open = false;
                this.config_open = false;
                this.log_open = !this.log_open;
            },

            histogramToggle() {
                this.log_open = false;
                this.config_open = false;
                this.timeline_open = false;
                this.spells_open = false;
                this.histogram_open = !this.histogram_open;
            },

            timelineToggle() {
                this.log_open = false;
                this.config_open = false;
                this.histogram_open = false;
                this.spells_open = false;
                this.timeline_open = !this.timeline_open;
            },

            spellsToggle() {
                this.log_open = false;
                this.config_open = false;
                this.histogram_open = false;
                this.timeline_open = false;
                this.spells_open = !this.spells_open;
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

            convertEnchants(enchants) {
                for (var slot in enchants)
                    enchants[slot] = this.convertEnchant(enchants[slot]);

                return enchants;
            },

            convertEnchant(id) {
                if (!id)
                    return id;

                var map = {
                    "46540": 27981,
                    "46538": 27982,
                    "46533": 27975,
                    "46532": 27968,
                    "46502": 27960,
                    "46504": 33990,
                    "46498": 27917,
                    "46496": 34001,
                    "46497": 27913,
                    "46514": 33997,
                    "46516": 33994,
                    "46512": 33993,
                    "46470": 34008,
                    "46518": 27924,
                };

                id = _.toString(id);
                if (map.hasOwnProperty(id))
                    id = map[id];

                return parseInt(id);
            },

            convertGems(gems) {
                for (var slot in gems)
                    gems[slot] = this.convertGemsSlot(slot, gems[slot]);

                return gems;
            },

            convertGemsSlot(slot, gems) {
                if (!this.hasSocketBonus(slot, gems)) {
                    var item = this.equippedItem(slot);
                    if (!item)
                        return gems;
                    var n = item.sockets ? item.sockets.length-1 : 0;
                    var arr = _.clone(gems);
                    for (var i=0; i<n; i++) {
                        arr.splice(n, 0, arr.splice(0, 1)[0]);
                        if (this.hasSocketBonus(slot, arr))
                            return arr;
                    }
                }

                return gems;
            },

            saveGear() {
                window.localStorage.setItem("magesim_tbc_equipped", JSON.stringify(this.equipped));
                window.localStorage.setItem("magesim_tbc_enchants", JSON.stringify(this.enchants));
                window.localStorage.setItem("magesim_tbc_gems", JSON.stringify(this.gems));
            },

            loadGear() {
                var equipped, enchants, gems;

                var str = window.localStorage.getItem("magesim_tbc_equipped");
                if (str) {
                    equipped = JSON.parse(str);
                    if (equipped)
                        _.merge(this.equipped, _.pick(equipped, _.keys(this.equipped)));
                }

                var str = window.localStorage.getItem("magesim_tbc_enchants");
                if (str) {
                    enchants = JSON.parse(str);
                    if (enchants) {
                        enchants = this.convertEnchants(enchants);
                        _.merge(this.enchants, _.pick(enchants, _.keys(this.enchants)));
                    }
                }

                var str = window.localStorage.getItem("magesim_tbc_gems");
                if (str) {
                    gems = JSON.parse(str);
                    if (gems) {
                        gems = this.convertGems(gems);
                        _.merge(this.gems, _.pick(gems, _.keys(this.gems)));
                    }
                }

                if (!equipped)
                    this.quickset(this.items.quicksets.t5_arcane_bis);
            },

            saveConfig() {
                window.localStorage.setItem("magesim_tbc_config", JSON.stringify(this.config));
            },

            loadConfig() {
                var str = window.localStorage.getItem("magesim_tbc_config");
                if (str) {
                    var config = JSON.parse(str);
                    if (config) {
                        _.merge(this.config, _.pick(config, _.keys(this.config)));
                        this.onLoadConfig(config);

                    }
                }
            },

            // Backwards compatibility
            onLoadConfig(cfg) {
                this.config.talents = this.conformTalents(this.config.talents);

                var timings = [
                    "trinket1", "trinket2",
                    "presence_of_mind", "arcane_power",
                    "icy_veins", "cold_snap",
                    "combustion", "berserking",
                    "drums", "bloodlust",
                    "mana_tide", "power_infusion",
                    "innervate",
                    "potion", "conjured",
                ];

                if (_.get(cfg, "innervate_at") === 0)
                    delete cfg.innervate_at;
                if (_.get(cfg, "conjured_at") === 0 && cfg.conjured == this.conjureds.CONJURED_MANA_GEM)
                    delete cfg.conjured_at;

                // Fire spec
                if (_.get(cfg, "spec") == 1) {
                    if (_.get(cfg, "fire_rotation") == 1)
                        this.config.main_rotation = this.main_rotations.MAIN_ROTATION_SC;
                    else
                        this.config.main_rotation = this.main_rotations.MAIN_ROTATION_FIB;
                }
                // Frost spec
                else if (_.get(cfg, "spec") == 2) {
                    this.config.main_rotation = this.main_rotations.MAIN_ROTATION_FRB;
                }
                // Arcane spec
                else if (!cfg.hasOwnProperty("main_rotation")) {
                    this.config.main_rotation = this.main_rotations.MAIN_ROTATION_AB;
                }

                if (!cfg.hasOwnProperty("targets"))
                    this.config.targets = 1;

                var from, to;
                for (var i=0; i<timings.length; i++) {
                    to = timings[i]+"_t";

                    from = timings[i]+"_at";
                    if (cfg.hasOwnProperty(from))
                        this.config[to][0] = cfg[from];

                    from = timings[i]+"_reuse_at";
                    if (cfg.hasOwnProperty(from))
                        this.config[to][1] = cfg[from];
                }
            },

            saveProfiles() {
                window.localStorage.setItem("magesim_tbc_profiles", JSON.stringify(this.profiles));
            },

            loadProfiles() {
                var str = window.localStorage.getItem("magesim_tbc_profiles");
                if (str) {
                    var profiles = JSON.parse(str);
                    if (profiles)
                        this.profiles = profiles;
                }
            },

            saveCustomItems() {
                window.localStorage.setItem("magesim_tbc_custom_items", JSON.stringify(this.customItems()));
            },

            loadCustomItems() {
                var str = window.localStorage.getItem("magesim_tbc_custom_items");
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
