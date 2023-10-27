class SimulationWorker {
    constructor(onSuccess, onError) {
        this.worker = new Worker("./dist/js/sim_worker.js?"+Math.random());

        this.worker.onerror = (...args) => {
            onError(...args);
            this.worker.terminate();
        };

        this.worker.onmessage = (event) => {
            var data = event.data;
            if (data.type == "error")
                onError(data);
            if (data.type == "success")
                onSuccess(data.result);
            this.worker.terminate();
        };
    }

    start(config, iterations) {
        config = _.cloneDeep(config);
        if (!config.dot_targets || config.dot_targets > config.targets)
            config.dot_targets = config.targets;
        this.worker.postMessage({
            type: "start",
            config: config,
            iterations: iterations ? iterations : 1,
        });
    }
}

class SimulationWorkers {
    constructor(iterations, onSuccess, onError) {
        this.threads = navigator.hardwareConcurrency || 2;
        this.workers = Array(this.threads);
        this.iterations = iterations;

        var sum = null;

        for (var i=0; i<this.threads; i++) {
            this.workers[i] = new SimulationWorker((result) => {
                if (!sum) {
                    sum = result;
                }
                else {
                    if (result.min_dps < sum.min_dps)
                        sum.min_dps = result.min_dps;
                    if (result.max_dps > sum.max_dps)
                        sum.max_dps = result.max_dps;
                    sum.avg_dps = (sum.avg_dps * sum.iterations + result.avg_dps * result.iterations) / (sum.iterations + result.iterations);
                    sum.t_gcd_capped = (sum.t_gcd_capped * sum.iterations + result.t_gcd_capped * result.iterations) / (sum.iterations + result.iterations);
                    sum.iterations+= result.iterations;
                    sum.n_oom+= result.n_oom;

                    if (result.histogram) {
                        for (var key in result.histogram) {
                            if (!sum.histogram.hasOwnProperty(key))
                                sum.histogram[key] = result.histogram[key];
                            else
                                sum.histogram[key]+= result.histogram[key];
                        }
                    }

                    if (result.all_results) {
                        var index = result.all_results.indexOf("\n");
                        sum.all_results+= result.all_results.substr(index+1);
                    }
                }

                if (sum.iterations == iterations)
                    onSuccess(sum);

            }, (error) => {
                console.error(error);
            });
        }
    }

    start(config) {
        config = _.cloneDeep(config);
        if (!config.dot_targets || config.dot_targets > config.targets)
            config.dot_targets = config.targets;
        var seed = config.rng_seed;
        for (var i=0; i<this.workers.length; i++) {
            var it = Math.floor(this.iterations/this.threads);
            var r = this.iterations%this.threads;
            if (r && i < r)
                it++;
            if (!it)
                break;
            if (config.rng_seed > 0)
                config.rng_seed+= it;
            this.workers[i].start(config, it);
        }
        config.rng_seed = seed;
    }
}

export { SimulationWorker, SimulationWorkers }
