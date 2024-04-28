<template>
  <div @mouseenter.stop="moveIn" @mouseleave="moveOut" class="tabbar">
    <van-notice-bar scrollable style="border-radius: 10px;width: 100%;" background="white" left-icon="volume-o"
      :mode="isShowX ? 'closeable' : ''" :text="currentNotify" />
  </div>
</template>
<script>
export default {
  data() {
    return {
      isShowX: false,
    };
  },
  props: ['currentNotify'],
  methods: {
    moveIn() {
      this.isShowX = true;
    },
    moveOut() {
      this.isShowX = false;
    },
  },
};
</script>

<style scoped>

::v-deep .van-icon {
  color: blue;
}
</style>
