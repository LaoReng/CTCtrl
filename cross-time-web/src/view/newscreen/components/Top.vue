<template>
  <div class="top">
    <div class="left">
      <span class="lbtn" @click="goHome">首页</span>
    </div>
    <div class="center">
      <!-- <div class="title">跨时空控制台可视化大数据平台</div> -->
      <div class="title">{{ setting.title }}可视化大数据平台</div>
    </div>
    <div class="right">
      <span>当前时间:{{ time }}</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import setting from "@/utils/setting";
import moment from "moment";
import { useRouter } from "vue-router";
import { ref, onMounted, onBeforeMount } from "vue";
//存储当前时间
let time = ref(moment().format("YYYY年MM月DD日 HH:mm:ss"));
let timer = ref(0);
const $router = useRouter();
const goHome = () => {
  $router.push("/");
};
//
onMounted(() => {
  setInterval(() => {
    time.value = moment().format("YYYY年MM月DD日 HH:mm:ss");
  }, 1000);
});
onBeforeMount(() => {
  clearInterval(timer.value);
});
</script>

<style scoped lang="scss">
.top {
  width: 100%;
  height: 100%;
  display: flex;
  user-select: none;
  .left {
    flex: 0.8;
    text-align: center;
    .lbtn {
      cursor: pointer;
      width: 150px;
      line-height: $screenHeader;
      height: 100%;
      color: #29fcff;
      font-size: 20px;
    }
    .lbtn:hover {
      color: #0084ff;
    }
  }
  .center {
    flex: 2;
    .title {
      width: 100%;
      height: 74px;
      text-align: center;
      line-height: 74px;
      color: #29fcff;
      line-height: 74px;
      font-size: 30px;
      background: -webkit-linear-gradient(
        135deg,
        rgb(20, 43, 188),
        #96a305 25%,
        #2e9614 50%,
        #e6d205 55%,
        #2cc4e0 60%,
        #d41b1b 80%,
        #ff6384 95%,
        #7708df
      );
      /* 文字颜色填充设置为透明 */
      -webkit-text-fill-color: transparent;
      /* 背景裁剪，即让文字使用背景色 */
      -webkit-background-clip: text;
      /* 背景图放大一下，看着柔和一些 */
      -webkit-background-size: 200% 100%;
      /* 应用动画flowCss 12秒速度 无限循环 线性匀速动画*/
      -webkit-animation: flowCss 10s infinite linear;
      @-webkit-keyframes flowCss {
        0% {
          /* 移动背景位置 */
          background-position: 0 0;
        }

        100% {
          background-position: -400% 0;
        }
      }
    }
  }

  .right {
    flex: 0.8;
    height: 100%;
    text-align: center;
    font-size: 20px;
    line-height: $screenHeader;
    color: #29fcff;
  }
}
</style>
