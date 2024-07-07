<template>
  <div
    v-show="isShow"
    @mouseenter.stop="moveIn"
    @mouseleave="moveOut"
    class="tabbar"
  >
    <van-notice-bar
      scrollable
      style="border-radius: 10px; width: 100%"
      background="#ecf9ff"
      left-icon="volume-o"
      :mode="isShowX ? 'closeable' : undefined"
      :text="content"
    />
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from "vue";
//是否显示X
let isShowX = ref(false);
//是否显示通知栏
let isShow = ref(true);
import noticeStore from "@/store/modules/notice/notice";
const store = noticeStore();
const moveIn = () => {
  isShowX.value = true;
};
const moveOut = () => {
  isShowX.value = false;
};
let content = ref();
const init = async () => {
  let res = await store.getNewNotice();
  //如果当前没有通知，那么就不显示通知栏
  if (res?.status == -2) {
    isShow.value = false;
  } else {
    content.value = res.content;
  }
};
onMounted(() => {
  init();
});
</script>

<style scoped>
::v-deep .van-icon {
  color: blue;
}
</style>
