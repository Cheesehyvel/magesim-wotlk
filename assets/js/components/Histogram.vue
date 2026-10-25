<template>
    <div class="histogram">
        <template v-if="chartData">
            <Bar :data="chartData" :options="chartOptions" />
        </template>
    </div>
</template>

<script>
    import { Bar } from 'vue-chartjs';
    import 'chart.js/auto';

    export default {
        components: { Bar },

        mounted() {
            this.draw();
        },

        watch: {
            data() {
                this.draw();
            }
        },

        props: ['data'],

        data() {
            return {
                chartData: null,
                chartOptions: null,
            }
        },

        methods: {
            draw() {
                var bin_size = 50;

                var data = {
                    labels: [],
                    datasets: [{
                        data: [],
                        backgroundColor: "#08f",
                        borderColor: "#05c",
                        borderWidth: 1,
                        barPercentage: 1.25,
                    }],
                };

                var keys = _.keys(this.data).sort((a,b) => a-b);
                var first = parseInt(keys[0]);
                var last = parseInt(_.last(keys));
                for (var i=first; i<=last; i+= bin_size) {
                    data.labels.push(i);
                    data.datasets[0].data.push(_.get(this.data, i.toString(), "0"));
                }
                data.labels.push(i);

                this.chartData = data;

                this.chartOptions = {
                    plugins: {
                        legend: {
                            display: false,
                        },
                        tooltip: {
                            enabled: false,
                        },
                    },
                    scales: {
                        x: {
                            display: true,
                            max: data.labels[data.labels.length-1],
                            title: {
                                display: true,
                                text: "DPS",
                            },
                            grid: {
                                color: "rgba(255,255,255,0.05)",
                            },
                            ticks: {
                                autoSkip: false,
                            }
                        },
                        y: {
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: "Iterations",
                            },
                            grid: {
                                color: "rgba(255,255,255,0.05)",
                            },
                            ticks: {
                                maxTicksLimit: 20,
                            }
                        }
                    }
                };
            }
        }
    }
</script>
