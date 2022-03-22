<template>
    <div :class="allClasses" :style="styles">
        <slot></slot>
    </div>
</template>

<script>
    export default {
        mounted() {
            this.$el.parentNode.classList.add("tooltip-anchor");
            this.$el.parentNode.addEventListener("mouseenter", this.checkCollision);
        },

        updated() {
            if (!this.$el.parentNode.classList.contains("tooltip-anchor")) {
                this.$el.parentNode.classList.add("tooltip-anchor");
                this.$el.parentNode.addEventListener("mouseenter", this.checkCollision);
            }
        },

        props: {
            position: {
                type: String,
                default: 'b'
            },
            styles: {
                type: Object,
                default: () => {}
            },
            classes: {
                type: Array,
                default: () => [],
            }
        },

        data() {
            return {
                is_open: false,
                pos: this.position.replace("top", "t").replace("left", "l").replace("right", "r").replace("bottom", "b")
            }
        },

        computed: {
            cssName() {
                return "position-"+this.pos;
            },

            allClasses() {
                var arr = ['tooltip'];
                arr = arr.concat(this.classes);
                arr.push("position-"+this.pos);
                if (this.is_open)
                    arr.push("open");
                return arr;
            }
        },

        methods: {
            open() {
                this.is_open = true;
                this.$nextTick(function() {
                    this.checkCollision();
                });
            },

            close() {
                this.is_open = false;
            },

            checkCollision() {
                var rect = this.$el.getBoundingClientRect();
                if (this.pos.indexOf("r") != -1 && rect.x + rect.width > document.body.offsetWidth)
                    this.pos = this.pos.replace("r", "l");
                else if (this.pos.indexOf("l") && rect.x < 0)
                    this.pos = this.pos.replace("l", "r");
            }
        },
    }
</script>