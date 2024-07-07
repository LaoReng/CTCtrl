<template>
  <div class="inner">
    <div class="item">
      <div class="item_element">
        <div style="display: flex; align-items: center; font-size: 30px">
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '2rem',
            }"
            :value="outputValue"
          />
          <p v-show="screenstore.isShowW" style="color: white">w</p>
          <p v-show="screenstore.isShowW" style="color: white; font-size: 20px">
            +
          </p>
        </div>
        <!-- <h4>2190</h4> -->
        <span>
          <i class="icon-dot" style="color: #006cff"></i>
          总访问量
        </span>
      </div>
      <div class="item_element">
        <div style="display: flex; align-items: center; font-size: 30px">
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '2rem',
            }"
            :value="weekCountValue"
          />
          <p v-show="screenstore.isShowW1" style="color: white">w</p>
          <p
            v-show="screenstore.isShowW1"
            style="color: white; font-size: 20px"
          >
            +
          </p>
        </div>
        <!-- <h4>2190</h4> -->
        <span>
          <i class="icon-dot" style="color: #006cff"></i>
          周访问量
        </span>
      </div>
      <div class="item_element">
        <div style="display: flex; align-items: center; font-size: 30px">
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '2rem',
            }"
            :value="currentCountValue"
          />
          <p v-show="screenstore.isShowW2" style="color: white">w</p>
          <p
            v-show="screenstore.isShowW2"
            style="color: white; font-size: 20px"
          >
            +
          </p>
        </div>
        <!-- <h4>2190</h4> -->
        <span>
          <i class="icon-dot" style="color: #006cff"></i>
          当前在线数
        </span>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from "vue";
import { useTransition } from "@vueuse/core";
import useScreenStore from "@/store/modules/screen/screen";
//引入仓库
const screenstore = useScreenStore();

//总访问数
const source: any = ref(0);
const outputValue = useTransition(source, {
  duration: 1400,
});
//周访问数
let weekCount = ref(0);
const weekCountValue = useTransition(weekCount, {
  duration: 1400,
});
//当前在线数
let currentCount = ref(0);
const currentCountValue = useTransition(currentCount, {
  duration: 1400,
});
//定时器
let timer: any = ref(null);
const init = async () => {
  //初始化不变的数据
  await screenstore.getSummaryInfo();
  //初始化在线人数
  await screenstore.getCurrentOnline();
  // console.log(Number(screenstore.dataSummaryInfo.visitTotal));
  //总访问数设定
  source.value = screenstore.getTotalVisit;
  //周访问量设定
  weekCount.value = screenstore.getWeekVisit;
  //在线数设定
  currentCount.value = screenstore.getCurrentOnlineCount;
  //实时刷新在线人数
  timer.value = setInterval(() => {
    screenstore.getCurrentOnline();
    currentCount.value = screenstore.getCurrentOnlineCount;
  }, 2500);
};
onMounted(() => {
  init();
});
onBeforeUnmount(() => {
  clearInterval(timer.value);
  timer.value = null;
});
// setInterval(() => {
//   source.value += 1;
// }, 1000);
</script>

<style scoped lang="scss">
.inner {
  width: 100%;
  justify-content: space-between;
  display: flex;
  align-items: center;

  .item {
    width: 100%;
    display: flex;
    justify-content: space-between;
    align-items: center;
    overflow: hidden;
    .item_element {
      margin-top: 20px;
      flex: 1;
      display: flex;
      flex-direction: column;
      overflow: hidden;
      align-items: center;
      .number {
        display: flex;
        align-items: center;
        font-size: 30px;
      }
    }
    span {
      margin-top: 20px;
      font-size: 1.2rem;
      color: #4c9bfd;
      align-items: center;
    }
  }
}
</style>
