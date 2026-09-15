import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import postcssMixins from "postcss-mixins";
import postcssNesting from "postcss-nesting";


export default defineConfig({
    worker: {
        format: "es",
    },
    plugins: [
        vue(),
    ],
    css: {
        postcss: {
            plugins: [
                postcssMixins,
                postcssNesting
            ],
        },
    },
});
