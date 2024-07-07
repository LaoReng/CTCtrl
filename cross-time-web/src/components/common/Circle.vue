<template>
  <div
    class="dot"
    :style="{ '--bg-color': color, '--border-color': borderColor }"
  >
    <span class="point"></span>
    <span class="wave1"></span>
    <span class="wave2"></span>
    <span class="wave3"></span>
    <span class="wave4"></span>
  </div>
</template>

<script setup>
import { defineProps } from "vue";
let { color, borderColor } = defineProps(["color", "borderColor"]);
</script>

<style scoped lang="scss">
.dot {
  position: relative;
  .point {
    width: 6px;
    height: 6px;
    border-radius: 100%;
    background-color: var(--bg-color);
    position: absolute;
    top: 50%;
    left: 50%;
    --bg-color: v-bind("color");
    --border-color: v-bind(borderColor);
    transform: translate(-50%, -50%);
  }
  .wave1,
  .wave2,
  .wave3,
  .wave4 {
    width: 0;
    height: 0;
    /*居中*/
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
  }
  .wave1 {
    animation: wave 2s 0s infinite;
  }
  .wave2 {
    animation: wave 2s 0.6s infinite;
  }
  .wave3 {
    animation: wave 2s 1.2s infinite;
  }
  .wave4 {
    animation: wave 2s 1.8s infinite;
  }
  @keyframes wave {
    0% {
      width: 15px;
      height: 15px;
      opacity: 0.4;
      border: 1px solid var(--border-color);
      border-radius: 100%;
      /* scale: 2; */
      background-color: var(--border-color);
    }
    100% {
      width: 20px;
      height: 20px;
      background-color: var(--border-color);
      opacity: 0;
      border: 1px solid var(--border-color);
      border-radius: 100%;
    }
  }
}
</style>
