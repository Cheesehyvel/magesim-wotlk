<template>
    <div class="timeline">
        <div class="graph">
            <canvas ref="canvas" height="80"></canvas>
        </div>
        <div class="events">
            <div class="timestamps">
                <div class="title">Effect (uptime %)</div>
                <div class="times">
                    <div class="time" v-for="time in times" :style="timeStyle(time)">
                        {{ formatTime(time) }}
                    </div>
                </div>
            </div>
            <div class="items">
                <div class="item" v-for="event in events">
                    <div class="title" :style="{color: event.color}">
                        {{ event.title }}
                        <template v-if="event.hasOwnProperty('uptime')">
                            ({{ event.uptime }}%)
                        </template>
                    </div>
                    <div class="times">
                        <div class="bar" v-for="item in event.events" :style="barStyle(item, event.color)"></div>
                    </div>
                </div>
            </div>
        </div>
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
                cds: [
                    { title: "Mana Tide", color: "#05c" },
                    { title: "Innervate", color: "#05c" },
                    { title: "Evocation", color: "#05c" },
                    { title: "Flame Cap", color: "rgba(200,120,70)" },
                    { title: "Drums of Battle", color: "rgba(160,160,60)" },
                    { title: "Drums of War", color: "rgba(160,160,60)" },
                    { title: "Drums of Restoration", color: "rgba(160,160,60)" },
                    { title: "Bloodlust", color: "rgba(220,70,70)" },
                    { title: "Icy Veins", color: "rgba(85,170,255)" },
                    { title: "Berserking", color: "rgba(200,80,80)" },
                    { title: "Arcane Power", color: "#48f" },
                    { title: "Power Infusion", color: "#dd0" },
                    { title: "Lightweave", color: "#823978" },
                    { title: "Black Magic", color: "#89d1d0" },
                    { title: "Hyperspeed Acceleration", color: "#206f80" },
                    { title: "Speed", color: "#d82" },
                    { title: "Wild Magic", color: "#cc6" },
                    { title: "Praxis", color: "#690042" },
                    { title: "Pushing the Limit", color: "#fcd3e1" },
                    { title: "Quad Core", color: "#ddd" },
                ],
                trinkets: [
                    { title: "Scale of Fates", color: "#ddd" },
                    { title: "Show of Faith", color: "#ddd" },
                    { title: "Memories of Love", color: "#55d" },
                    { title: "Alacrity of the Elements", color: "#ddd" },
                    { title: "Dying Curse", color: "#5b5" },
                    { title: "Now is the Time!", color: "#ddd" },
                    { title: "Embrace of the Spider", color: "#777" },
                    { title: "Twilight Serpent", color: "#ddd" },
                    { title: "Tome of Arcane Phenomena", color: "#ddd" },
                    { title: "Argent Valor", color: "#ddd" },
                    { title: "Mark of the War Prisoner", color: "#ddd" },
                    { title: "Forge Ember", color: "#ddd" },
                    { title: "Improved Mana Gem", color: "#2da" },
                    { title: "Fel Infusion", color: "#b24" },
                    { title: "Mojo Madness", color: "#d22" },
                    { title: "Insight of the Ashtongue", color: "#cc8" },
                    { title: "Power Circle", color: "#a2a" },
                ],
                mana_gains: [
                    { title: "Mana Gem", color: "rgba(85,255,85)" },
                    { title: "Mana Potion", color: "#00d" },
                ]
            }
        },

        computed: {
            times() {
                var times = [];
                var steps = [1,2,5,10,15,20,30,40,60,90,120];
                var step = 150;
                var div = this.result.t / 20;

                for (var i=0; i<steps.length; i++) {
                    if (div < steps[i]) {
                        step = steps[i];
                        break;
                    }
                }

                for (var i=step; i<this.result.t; i+= step) {
                    times.push(i);
                }

                return times;
            },

            events() {
                var self = this;
                var events = [];
                var event, start, end, uptime;
                var logs;

                // CDs
                for (var i=0; i<this.cds.length; i++) {
                    logs = _.filter(this.result.log, l => l.text.indexOf(this.cds[i].title) > 0);
                    if (logs.length) {
                        uptime = 0;
                        event = _.clone(this.cds[i]);
                        event.events = [];
                        while (logs.length) {
                            start = logs.shift();
                            for (end = logs.shift(); end && end.text.indexOf("Player lost ") != 0; end = logs.shift());
                            event.events.push({
                                start: start.t,
                                end: end ? end.t : this.result.t,
                            });
                            uptime+= (end ? end.t : this.result.t) - start.t;
                        }
                        event.uptime = Math.round(uptime / this.result.t * 100);
                        events.push(event);
                    }
                }

                // Trinkets
                var delta = 0;
                for (var i=0; i<this.trinkets.length; i++) {
                    logs = _.filter(this.result.log, l => l.text.indexOf(this.trinkets[i].title) > 0);
                    if (logs.length) {
                        uptime = 0;
                        event = _.clone(this.trinkets[i]);
                        event.events = [];
                        while (logs.length) {
                            start = logs.shift();
                            for (end = logs.shift(); end && end.text.indexOf("Player lost ") != 0; end = logs.shift());
                            event.events.push({
                                start: start.t,
                                end: end ? end.t : this.result.t,
                            });
                            uptime+= (end ? end.t : this.result.t) - start.t;
                        }
                        event.uptime = Math.round(uptime / this.result.t * 100);
                        events.push(event);
                    }
                }

                // Mana gains
                var delta = 0;
                for (var i=0; i<this.mana_gains.length; i++) {
                    start = _.filter(this.result.log, function(a) { return a.text.indexOf(" mana from "+self.mana_gains[i].title) > 0; });
                    if (start.length) {
                        event = _.clone(this.mana_gains[i]);
                        event.events = [];
                        for (var j=0; j<start.length; j++) {
                            event.events.push({
                                start: start[j].t,
                                end: start[j].t+0.5,
                            });
                        }
                        events.push(event);
                    }
                }

                return events;
            },
        },

        methods: {
            timeStyle(time) {
                return {
                    left: (time / this.result.t * 100) + "%",
                }
            },

            barStyle(item, color) {
                return {
                    left: (item.start / this.result.t * 100) + "%",
                    right: (100 - (item.end / this.result.t * 100)) + "%",
                    background: color,
                };
            },

            formatTime(time) {
                return time+"s";

                // var s = time%s;
                // var m = time - s;
                // return (m < 10 ? "0"+m : m) + ":" + (s < 10 ? "0"+s : s);
            },

            draw() {
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
                            },
                            gridLines: {
                                color: "rgba(120,140,240,0.1)",
                            }
                        }],
                        yAxes: [{
                            type: "linear",
                            ticks: {
                                beginAtZero: true,
                            },
                            scaleLabel: {
                                display: true,
                                labelString: "Mana (%)",
                            },
                            gridLines: {
                                color: "rgba(120,140,240,0.4)",
                            }
                        }, {
                            id: "dps",
                            type: "linear",
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
                    if (!d.length) {
                        for (var i=0; i<this.result.log.length; i++) {
                            if (this.result.log[i].t < 0)
                                continue;
                            if (this.result.log[i].text.indexOf("Mana Regen") != -1)
                                d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                        }
                    }
                    d.push({x: this.result.t, y: _.last(this.result.log.filter(l => l.unit == "Player")).mana_percent});
                }
                else {
                    for (var i=0; i<this.result.log.length; i++) {
                        if (this.result.log[i].t < 0)
                            continue;
                        d.push({x: this.result.log[i].t, y: this.result.log[i].mana_percent});
                    }
                }
                data.datasets.push({
                    data: d,
                    borderColor: "#08f",
                    borderWidth: 1,
                    pointRadius: 0,
                    hitRadius: 0,
                    label: "Mana",
                });

                // DPS
                d = [];
                d.push({x: 0, y: 0});
                for (var i=0; i<this.result.log.length; i++) {
                    if (this.result.log[i].type == 3 && this.result.log[i].t)
                        d.push({x: this.result.log[i].t, y: this.result.log[i].dmg / this.result.log[i].t});
                }
                d.push({x: this.result.t, y: this.result.dps});
                data.datasets.push({
                    data: d,
                    borderColor: "#f00",
                    borderWidth: 1,
                    pointRadius: 0,
                    label: "DPS",
                    fill: false,
                    yAxisID: "dps",
                });

                this.renderChart(data, options);
            },
        }
    }
</script>