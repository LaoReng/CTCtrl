<template>
  <div>
    <el-progress type="dashboard" :percentage="percentage" :color="colors">
      <template #default="{ percentage }">
        <span class="percentage-value">{{ percentage }}%</span>
        <span class="percentage-label">{{ perlabel }}</span>
      </template>
    </el-progress>
  </div>
</template>

<script setup lang="ts">
defineProps(["percentage", "perlabel", "colors"]);
</script>

<style scoped></style>
