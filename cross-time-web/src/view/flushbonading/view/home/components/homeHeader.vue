<template>
  <div class="container">
    <div class="left">
      <p class="timer">{{ time }}</p>
      <p class="date">{{ ymd }}{{ filterWeek(week) }}</p>
    </div>
    <div class="right">
      <div>天气晴</div>
      <img
        width="100%"
        src="https://assets.msn.cn/weathermapdata/1/static/weather/Icons/taskbar_v10/Condition_Card/SunnyDayV3.svg"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from "vue";
import moment from "moment";
let ymdr: any = ref(0); //存储时间总情况
let ymd = ref(moment().format("YYYY年MM月DD日"));
let time = ref(moment().format("HH:mm:ss"));
//以星期显示
let week = ref(moment().format("E"));
//星期格式化
const filterWeek = (key: any) => {
  switch (key) {
    case 1:
      week.value = "星期一";
      return week.value;
    case 2:
      week.value = "星期二";
      return week.value;
    case 3:
      week.value = "星期三";
      return week.value;
    case 4:
      week.value = "星期四";
      return week.value;
    case 5:
      week.value = "星期五";
      return week.value;
    case 6:
      week.value = "星期六";
      return week.value;
    default:
      week.value = "星期日";
      return week.value;
  }
};
onMounted(() => {
  ymdr.value = setInterval(() => {
    ymd.value = moment().format("YYYY年MM月DD日");
    time.value = moment().format("HH:mm:ss");
    week.value = moment().format("E");
  }, 1000);
});
onBeforeUnmount(() => {
  clearInterval(ymdr.value);
});
</script>

<style scoped lang="scss">
.container {
  display: flex;
  .left {
    // background-color: blue;
    flex: 1;

    // background-color: red;
    font-family: "Lucida Sans", "Lucida Sans Regular", "Lucida Grande", Geneva;
    text-align: left;
    color: cornflowerblue;
    font-weight: 700;
    .timer {
      font-size: 1.4rem;
    }
    .date {
      font-size: 0.7rem;
      margin-top: 0.5rem;
    }
  }
  .right {
    text-align: center;
    font-size: 0.7rem;
    font-family: "Lucida Sans", "Lucida Sans Regular", "Lucida Grande", Geneva;
    flex: 0.3;
    // background-color: red;
  }
}
</style>
