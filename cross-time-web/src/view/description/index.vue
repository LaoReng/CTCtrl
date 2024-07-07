<template>
  <div
    v-show="isShow"
    class="toTop"
    @mouseenter="scrollTrigger = true"
    @mouseleave="scrollTrigger = false"
    @click="goBack"
  >
    <svg
      t="1719023197877"
      class="icon"
      viewBox="0 0 1024 1024"
      version="1.1"
      xmlns="http://www.w3.org/2000/svg"
      p-id="4540"
      width="16"
      height="16"
    >
      <path
        d="M946.494 754.216c19.445-19.445 19.445-51.265 0-70.711L547.355 284.368c-19.445-19.445-51.265-19.445-70.711 0L77.506 683.506c-19.445 19.445-19.445 51.265 0 70.711"
        p-id="4541"
        :fill="scrollTrigger ? '#ffff' : '#337ab7'"
      ></path>
    </svg>
  </div>
  <div class="desc-container">
    <div class="common-layout" ref="descContainer">
      <el-container>
        <!-- 头部 -->
        <el-header>
          <Top />
        </el-header>
        <!-- 内容区域 -->
        <el-container>
          <!-- 侧边栏 -->
          <NewSliderItem @child-click="SonToParent" />
          <!-- 主体 -->
          <el-main>
            <Content1 v-show="currentClickItem?.id == 1" />
            <Content2 v-show="currentClickItem?.id == 2" />
            <Content3 v-show="currentClickItem?.id == 3" />
            <Content4 v-show="currentClickItem?.id == 4" />
            <Content5 v-show="currentClickItem?.id == 5" />
          </el-main>
        </el-container>
      </el-container>
    </div>
  </div>
</template>

<script setup lang="ts">
// import SliderItem from "./slider/index.vue";
import NewSliderItem from "./newslider/index.vue";
import Top from "./top/index.vue";
import Content1 from "./content/process1.vue";
import Content2 from "./content/process2.vue";
import Content3 from "./content/process3.vue";
import Content4 from "./content/process4.vue";
import Content5 from "./content/process5.vue";
import { ref } from "vue";
let scrollTop = ref(0); //默认距离顶部的距离
let isShow = ref(false); //控制返回顶部的显隐
let scrollTrigger = ref(false); //默认初始值
const descContainer = ref(null);
let currentClickItem: any = ref();
let body = document.querySelector("body");
body?.addEventListener("scroll", (e: any) => {
  let clientHeight = document.documentElement.clientHeight;
  if (clientHeight < e.target?.scrollTop) {
    isShow.value = true;
  } else {
    isShow.value = false;
  }
});

//回到顶部
const goBack = () => {
  //获取当前距离顶部的距离
  scrollTop.value =
    document.documentElement.scrollTop || document.body.scrollTop;
  // console.log(scrollTop.value);
  let steep = scrollTop.value / 200;
  // 滚动到顶部
  let timer = setInterval(() => {
    scrollTop.value -= steep;
    steep += 20;
    if (scrollTop.value <= 0) {
      clearInterval(timer);
      isShow.value = false;
    }
    document.body.scrollTop = scrollTop.value;
    document.documentElement.scrollTop = scrollTop.value;
  }, 30);
};
// 子组件调用
const SonToParent = (item: any) => {
  currentClickItem.value = item;
};
</script>

<style scoped lang="scss">
.desc-container {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100vw;
  min-width: 70rem;
  height: 100%;
  background: linear-gradient(to bottom, #000000, #ebebeb);
  .common-layout {
    width: 80%;
  }
  .el-header {
    margin-top: 1rem;
    background-color: white;
    border-radius: 1rem;
    height: 12rem;
  }
  .el-container {
    .el-container {
      margin-top: 1rem;
      .el-main {
        border: 1px solid white;
        background-color: white;
        border-radius: 1rem;
        margin-left: 1rem;
        // height: calc(100vh - 14rem);
      }
    }
  }
}
.toTop {
  position: absolute;
  color: white;
  right: 5%;
  bottom: 5%;
  background-color: rgb(220, 226, 233);
  width: 2.5rem;
  height: 2.5rem;
  border-radius: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 111;
}
.toTop:hover {
  background-color: rgb(6, 106, 201);

  animation: 0.5s all ease-in-out;
}
</style>
