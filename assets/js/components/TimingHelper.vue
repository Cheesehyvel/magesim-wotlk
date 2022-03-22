<template>
    <help>
        Enter timings in order.
        <div v-if="!nocd">
            If a timing is empty or lower than the CD it will be used when ready.
        </div>
        <div><slot></slot></div>
    </help>
</template>

<script>
    export default {
        props: ['nocd'],
    }
</script>