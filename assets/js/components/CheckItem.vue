<template>
    <div class="check-item" :class="[value ? 'checked' : 'unchecked']">
        <span class="material-icons" v-html="icon"></span>
        <span class="check-text"><slot></slot></span>
    </div>
</template>

<script>
    export default {
        props: ['value'],

        computed: {
            icon() {
                return this.value ? '&#xe876' : '&#xe5cd';
            }
        }
    }
</script>