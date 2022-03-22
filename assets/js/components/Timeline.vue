<template>
    <div class="graph values">
        <canvas ref="canvas"></canvas>
    </div>
</template>

<script>
    import { Line } from 'vue-chartjs';

    export default {
        mixins: [Line],

        mounted() {
            this.addPlugin({
                id: "custom-plugin",
                afterRender: this.afterRender,
            });
            this.draw();
        },

        watch: {
            result() {
                this.draw();
            }
        },

        props: ['result'],

        data() {
            return {
                has_drawn: false,
                buff_padding: 1.5,
                buff_start_pos: 2,
                cds: [
                    { title: "Bloodlust", color: "rgba(255,70,70,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_bloodlust.jpg" },
                    { title: "Icy Veins", color: "rgba(85,170,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_frost_coldhearted.jpg" },
                    { title: "Arcane Power", color: "rgba(221,51,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_lightning.jpg" },
                    { title: "Band of Eternal Sage", color: "rgba(255, 128, 0, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_ring_55.jpg" },
                    { title: "Destruction", color: "rgba(153, 51, 221, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_potion_107.jpg" },
                    { title: "Mana Tide", color: "rgba(200, 200, 200, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_frost_summonwaterelemental.jpg" },
                    { title: "Power Infusion", color: "rgba(255, 255, 0, 0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_powerinfusion.jpg" },
                    { title: "Drums of Battle", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_02.jpg" },
                    { title: "Drums of War", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_03.jpg" },
                    { title: "Drums of Restoration", color: "rgba(160,160,60,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_drum_07.jpg" },
                    { title: "Innervate", color: "rgba(0,0,0,255,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_lightning.jpg" },
                    { title: "Evocation", color: "rgba(105,221,105,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/spell_nature_purge.jpg" },
                ],
                trinkets: [
                    { title: "Fel Infusion", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_bone_elfskull_01.jpg" },
                    { title: "Burst of Knowledge", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_amulet_07.jpg" },
                    { title: "Silver Crescent", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas06.jpg" },
                    { title: "Essence of the Martyr", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas01.jpg" },
                    { title: "Serpent Coil", img: "https://wow.zamimg.com/images/wow/icons/large/spell_arcane_arcanetorrent.jpg" },
                    { title: "Dark Iron Pipeweed", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_pipe_01.jpg" },
                    { title: "Recurring Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Essence of Sapphiron", img: "https://wow.zamimg.com/images/wow/icons/large/inv_trinket_naxxramas06.jpg" },
                    { title: "Spell Haste", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_searinglight.jpg" },
                    { title: "Unstable Currents", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Spell Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Focused Power", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Enlightenment", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_gem_pearl_04.jpg" },
                    { title: "Arcane Energy", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Crimson Serpent", img: "https://wow.zamimg.com/images/wow/icons/large/ability_hunter_pet_windserpent.jpg" },
                    { title: "Mojo Madness", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_head_troll_01.jpg" },
                    { title: "Aura of the Crusade", img: "https://wow.zamimg.com/images/wow/icons/large/spell_shadow_siphonmana.jpg" },
                    { title: "Insight of the Ashtongue", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_elvencoins.jpg" },
                    { title: "Call of the Nexus", img: "https://wow.zamimg.com/images/wow/icons/large/spell_holy_mindvision.jpg" },
                    { title: "Power Circle", img: "https://wow.zamimg.com/images/wow/icons/large/inv_jewelry_talisman_15.jpg" },
                ],
                mana_gains: [
                    { title: "Mana Gem", color: "rgba(85,255,85,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_misc_gem_stone_01.jpg" },
                    { title: "Mana Potion", color: "rgba(255,255,0,0.6)", img: "https://wow.zamimg.com/images/wow/icons/large/inv_potion_137.jpg" },
                ]
            }
        },

        methods: {
            afterRender() {
                this.drawImages();
            },

            draw() {
                this.has_drawn = false;
                var self = this;

                var data = {
                    datasets: [],
                };

                var options = {
                    legend: {
                        display: true,
                        labels: {
                            filter: function(item, chart) {
                                return item.text != "";
                            }
                        }
                    },
                    tooltips: {
                        enabled: false,
                    },
                    elements: {
                        line: {
                            tension: 0
                        }
                    },
                    scales: {
                        xAxes: [{
                            type: "linear",
                            ticks: {
                                max: this.result.t,
                            },
                            scaleLabel: {
                                display: true,
                                labelString: "Time (s)",
                            }
                        }],
                        yAxes: [{
                            type: "linear",
                            scaleLabel: {
                                display: true,
                                labelString: "Mana (%)",
                            }
                        }, {
                            id: "dps",
                            type: "linear",
                            position: "right",
                            ticks: {
                                beginAtZero: true,
                            },
                            scaleLabel: {
                                display: true,
                                labelString: "DPS",
                            }
                        }]
                    }
                };

                // Mana
                var d = [];
                var mana_smooth = true;
                if (mana_smooth) {
                    for (var i=0; i<this.result.log.length; i++) {
                        if (this.result.log[i].text.indexOf("Vampiric Touch") != -1)
                            d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                    }
                    if (!d.length) {
                        for (var i=0; i<this.result.log.length; i++) {
                            if (this.result.log[i].text.indexOf("Mana Regen") != -1)
                                d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                        }
                    }
                    d.push({x: this.result.t, y: _.last(this.result.log).mana_percent});
                }
                else {
                    for (var i=0; i<this.result.log.length; i++)
                        d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                }
                data.datasets.push({
                    data: d,
                    borderColor: "#08f",
                    borderWidth: 0.5,
                    pointRadius: 0,
                    hitRadius: 0,
                    label: "Mana",
                });


                // DPS
                d = [];
                d.push({x: 0, y: 0});
                for (var i=0; i<this.result.log.length; i++) {
                    if (this.result.log[i].type == 1 && this.result.log[i].t)
                        d.push({x: this.result.log[i].t, y: this.result.log[i].dmg / this.result.log[i].t});
                }
                d.push({x: this.result.t, y: this.result.dps});
                data.datasets.push({
                    data: d,
                    borderColor: "#f00",
                    borderWidth: 0.5,
                    pointRadius: 0,
                    label: "DPS",
                    fill: false,
                    yAxisID: "dps",
                });

                var delta = 0;
                var start, end;
                var buff_width = 5;

                // CDs
                for (var i=0; i<this.cds.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+this.cds[i].title});
                    end = _.filter(this.result.log, {text: "Lost "+this.cds[i].title});
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: end.length > j ? end[j].t : this.result.t, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: this.cds[i].color,
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            // label: j == 0 ? this.cds[i].title : '',
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta++;
                }

                // Trinkets
                var trinket_colors = ["rgba(255,255,255,0.6)", "rgba(160,60,160,0.6)"];

                var t = 0;
                for (var i=0; i<this.trinkets.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+this.trinkets[i].title});
                    end = _.filter(this.result.log, {text: "Lost "+this.trinkets[i].title});
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: end.length > j ? end[j].t : this.result.t, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: trinket_colors[t],
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            // label: j == 0 ? this.trinkets[i].title : '',
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length) {
                        delta++;
                        t++;
                        if (t > 1)
                            break;
                    }
                }

                // Mana gained
                /*
                for (var i=0; i<this.mana_gains.length; i++) {
                    start = _.filter(this.result.log, function(a) { return a.text.indexOf(" mana from "+self.mana_gains[i].title) > 0; });
                    for (var j=0; j<start.length; j++) {
                        data.datasets.push({
                            data: [
                                {x: start[j].t, y: delta*this.buff_padding + this.buff_start_pos},
                                {x: start[j].t+1, y: delta*this.buff_padding + this.buff_start_pos}
                            ],
                            borderColor: this.mana_gains[i].color,
                            borderWidth: buff_width,
                            pointRadius: 0,
                            hitRadius: 0,
                            label: '',
                            fill: false,
                        });
                    }
                    if (start.length)
                        delta++;
                }
                */

                this.renderChart(data, options);
            },

            drawImages() {
                var self = this;

                var x, y, start;
                var delta = 0;
                var px = 12;

                var buffs = _.concat(this.cds, this.trinkets);
                for (var i=0; i<buffs.length; i++) {
                    start = _.filter(this.result.log, {text: "Gained "+buffs[i].title});
                    if (buffs[i].img) {
                        for (var j=0; j<start.length; j++) {
                            x = this.$data._chart.scales["x-axis-0"].getPixelForValue(start[j].t) - px;
                            y = this.$data._chart.scales["y-axis-0"].getPixelForValue(delta*this.buff_padding + this.buff_start_pos) - px/2;
                            var im = new Image;
                            im.onload = (function(xx, yy, img) {
                                setTimeout(function() { self.$refs.canvas.getContext("2d").drawImage(img, xx, yy, 12, 12) }, 100);
                            }(x, y, im));
                            im.src = buffs[i].img;
                        }
                    }
                    if (start.length)
                        delta++;
                }

                for (var i=0; i<this.mana_gains.length; i++) {
                    start = _.filter(this.result.log, function(a) { return a.text.indexOf(" mana from "+self.mana_gains[i].title) > 0; });
                    if (this.mana_gains[i].img) {
                        for (var j=0; j<start.length; j++) {
                            x = this.$data._chart.scales["x-axis-0"].getPixelForValue(start[j].t) - px;
                            y = this.$data._chart.scales["y-axis-0"].getPixelForValue(delta*this.buff_padding + this.buff_start_pos) - px/2;
                            var im = new Image;
                            im.onload = (function(xx, yy, img) {
                                setTimeout(function() { self.$refs.canvas.getContext("2d").drawImage(img, xx, yy, 12, 12) }, 100);
                            }(x, y, im));
                            im.src = this.mana_gains[i].img;
                        }
                    }
                    if (start.length)
                        delta++;
                }
            },
        }
    }
</script>