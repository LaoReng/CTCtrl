<!-- eslint-disable @typescript-eslint/no-unused-vars -->
<template>
  <div class="app-header">
    <div class="left">
      <img src="@/assets/images/logo.jpg" />
      <span class="title">跨时空控制台</span>
    </div>
    <div class="center">
      <!-- 在element-plus是el-countdown,不是el-statistic -->
      <el-countdown
        @change="currentDownTime"
        :value-style="changeColor ? { color: 'red' } : { color: 'white' }"
        @finish="surplusTime"
        :value="vncstore.deadLine"
        format="HH:mm:ss"
        time-indices
        title="🚩剩余时间"
      ></el-countdown>
    </div>
    <div class="right">
      <div class="btn" @click="finishControl">
        <svg
          t="1716524085812"
          class="icon"
          viewBox="0 0 1024 1024"
          version="1.1"
          xmlns="http://www.w3.org/2000/svg"
          p-id="3209"
          width="32"
          height="32"
        >
          <path
            d="M511.1 923.8C295 923.8 119.2 748 119.2 531.9c0-108.8 45.8-213.6 125.7-287.6 24-22.2 61.4-20.8 83.6 3.2 22.2 24 20.8 61.4-3.2 83.6-55.8 51.6-87.7 124.8-87.7 200.8 0 150.9 122.7 273.6 273.6 273.6s273.6-122.7 273.6-273.6c0-76.7-30.9-147.8-87-200.1-23.9-22.3-25.2-59.7-2.9-83.6 22.3-23.9 59.7-25.2 83.6-2.9C857.6 319.1 903 423.6 903 531.9c0 216.1-175.8 391.9-391.9 391.9z"
            fill="#d81e06"
            p-id="3210"
          ></path>
          <path
            d="M516.7 663.3c-32.5 0-59.2-26.6-59.2-59.2V160.5c0-32.5 26.6-59.2 59.2-59.2 32.5 0 59.2 26.6 59.2 59.2v443.7c-0.1 32.5-26.7 59.1-59.2 59.1z"
            fill="#d81e06"
            p-id="3211"
          ></path>
        </svg>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
// import axios from "axios";
import { useRouter } from "vue-router";
import useVncStore from "@/store/modules/vnc/vnc";
import useHomeStore from "@/store/modules/home/home";
let vncstore = useVncStore();
let homestore = useHomeStore();
let $router = useRouter();
//倒计时变色
let changeColor = ref<boolean>(false);
//截止时间

//倒计时变色的回调
const currentDownTime = (val: number) => {
  vncstore.count = Number(val);

  //小于59s
  if (Math.floor((val - 1000 * 60 * 1) / 1000) < 0) {
    changeColor.value = true; //
  }
};
//结束时间触发
const surplusTime = async (val: number) => {
  console.log("%c触发倒计时了吗", "color:red");
  console.log(val);
  if (!val) {
    await vncstore.commonHandleFinish();
    console.log("%c触发了", "color:green");
    $router.push({
      path: "/home",
    });
    homestore.init();
  }
};
//点击结束控制
const finishControl = () => {
  vncstore.finishControl();
};

//刷新页面触发的事件/关闭页面的触发
window.onbeforeunload = async () => {
  let obj = Object.assign(
    {},
    JSON.parse(localStorage.getItem("startEquipment") as string),
  );
  obj.CtrlTime = vncstore.count;
  // console.log(obj);
  localStorage.setItem("startEquipment", JSON.stringify(obj));
};
//页面关闭事件
window.onunload = async () => {
  finishControl();
};
</script>

<style scoped lang="scss">
/* 深度选择器 */
.el-statistic >>> .title {
  color: white;
}

.el-statistic >>> .head .title {
  font-size: 1rem;
}

.app-header {
  -webkit-user-select: none;
  /*谷歌 /Chrome*/
  -moz-user-select: none;
  /*火狐/Firefox*/
  -ms-user-select: none;
  /*IE 10+*/
  user-select: none;
  background-color: rgb(173, 197, 255);
  display: flex;
  height: 100%;

  .left {
    height: 100%;
    display: flex;
    flex: 1;
    align-items: center;

    img {
      -webkit-user-drag: none;
      width: 4rem;
      height: 100%;
    }

    .title {
      -webkit-user-drag: none;
      width: 10rem;
      font-size: 1.2rem;
      color: white;
    }
  }

  .center {
    width: 100%;
    height: 100%;
    flex: 1;
    display: flex;
    justify-content: center;
    align-items: center;
  }

  .right {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: flex-end;
    text-align: right;
    margin-right: 1rem;
  }
}
</style>
