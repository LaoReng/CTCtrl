<template>
  <div v-if="screenStore.isShowScreen">加载中</div>
  <div v-else class="ViewContainer">
    <div class="screen" ref="screen">
      <!-- 当前设备状态 -->
      <div class="top">
        <Top />
      </div>
      <div class="content">
        <!-- 左侧 -->
        <div class="left">
          <!-- 左上 -->
          <div class="left_top">
            <LeftTop />
          </div>
          <!-- 左中 -->
          <div class="left_center">
            <LeftCenter />
          </div>
          <!-- 左下 -->
          <div class="left_bottom">
            <LeftBottom />
          </div>
        </div>
        <!-- 中间 -->
        <div class="center">
          <CenterCenter />
        </div>
        <!-- 右 -->
        <div class="right">
          <div class="right_top">
            <Right_Top />
          </div>
          <div class="right_center">
            <Right_Center />
          </div>
          <div class="right_bottom">
            <Right_Bottom />
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import Top from "./components/Top.vue";
import LeftTop from "./components/Left_Top.vue";
import LeftCenter from "./components/Left_Center.vue";
import LeftBottom from "./components/Left_Bottom.vue";
import CenterCenter from "./components/Center_Center.vue";
import Right_Top from "./components/Right_Top.vue";
import Right_Center from "./components/Right_Center.vue";
import Right_Bottom from "./components/Right_Bottom.vue";
import { onMounted, onBeforeUnmount, ref } from "vue";
import ScreenStore from "@/store/modules/screen/screen";
const screenStore = ScreenStore();
function getScale(w = 1920, h = 1080) {
  const ww = window.innerWidth / w;
  const wh = window.innerHeight / h;
  return ww < wh ? ww : wh;
}

//获取数据大屏展示内容盒子的DOM元素
let screen = ref();
let event: any = ref(window);
const windowResize = () => {
  screen.value!.style.transform = `scale(${getScale()}) translate(-50%,-50%)`;
};
const listenDOMResize = () => {
  screen.value!.style.transform = `scale(${getScale()}) translate(-50%,-50%)`;
  //监听视口的变化
  window.addEventListener("resize", windowResize);
};
onMounted(() => {
  console.log("%c数据大屏初次加载完成", "color:blue");
  listenDOMResize();
});
onBeforeUnmount(() => {
  //转变成其他页面的时候销毁
  if (event.value) {
    window.removeEventListener("resize", windowResize);
    event.value = null;
  }
});
</script>

<style scoped lang="scss">
.ViewContainer {
  width: 100vw;
  height: 100vh;
  max-width: 100vw;
  max-height: 100vh;
  overflow: hidden;
  background-color: #101129;
  .screen {
    overflow: hidden;
    position: fixed;
    left: 50%;
    top: 50%;
    width: 1920px;
    height: 1080px;
    transform-origin: left top;
    .top {
      position: relative;
      width: 100%;
      height: 60px;
      border-bottom: 1px solid rgb(104, 216, 254);
      // background: #1d1f20;
      background-color: rgb(16, 17, 41);
    }
    .content {
      // background-color: blue;
      height: calc(100% - $screenHeader);
      display: flex;
      .left {
        flex: 1;
        max-width: 28rem;
        display: flex;
        flex-direction: column;
        .left_top {
          flex: 0.2;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 20px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
        .left_center {
          flex: 1;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 20px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
        .left_bottom {
          flex: 0.5;
          color: white;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 20px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
      }
      .center {
        flex: 2.8;
      }
      .right {
        flex: 1;
        display: flex;
        flex-direction: column;
        .right_top {
          flex: 1;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 10px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
        .right_center {
          flex: 0.8;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 20px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
        .right_bottom {
          flex: 1;
          background: $screenBorderBottom;
          background-repeat: no-repeat;
          background-size:
            3px 24px,
            24px 3px;
          padding: 20px;
          border-bottom: 2px solid rgb(11, 58, 161);
        }
      }
    }
  }
}
</style>
