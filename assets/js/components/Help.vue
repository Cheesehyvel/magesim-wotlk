<template>
    <div class="help">
        <div class="material-icons" :class="[outlined ? 'material-symbols-outlined' : '']" v-html="iconCode"></div>
        <tooltip :position="position">
            <slot></slot>
        </tooltip>
    </div>
</template>

<script>
    export default {
        props: {
            position: {
                type: String,
                default: 'r'
            },

            icon: {
                type: String,
                default() {
                    return "e8fd";
                }
            },

            outlined: {
                type: Boolean,
                default: false,
            }
        },

        computed: {
            iconCode() {
                return "&#x"+this.icon+";";
            }
        }
    }
</script>