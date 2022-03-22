let mix = require("laravel-mix");

mix.options({
    processCssUrls: false
});

mix.js("assets/js/app.js", "js")
    .js("assets/js/sim_worker.js", "js")
    .sass("assets/sass/app.scss", "css")
    .setPublicPath("dist")
    .vue();