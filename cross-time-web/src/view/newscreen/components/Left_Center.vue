<template>
  <div class="inner">
    <div class="title">设备登录历史</div>
    <div class="content" v-if="screenstore.getHistoryList.length > 0">
      <div class="head">
        <div class="col1">登录时间</div>
        <div class="col2">设备地址</div>
        <div class="col3">登录IP</div>
      </div>
      <div class="Mylist">
        <div class="row" v-for="i in screenstore.getHistoryList" :key="i.area">
          <el-tooltip effect="light" :hide-after="0" placement="top">
            <template #content>
              登录时间：{{ i.date }}
              <br />
              地址：{{ i.address }}
              <br />
              IP：{{ i.ip }}
            </template>
            <div class="list">
              <div class="col1">{{ i.date }}</div>
              <div class="col2">{{ i.address }}</div>
              <div class="col3">{{ i.ip }}</div>
            </div>
          </el-tooltip>
        </div>
      </div>
    </div>
    <div v-else>
      <el-empty description="加载中..." />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from "vue";
import useScreenStore from "@/store/modules/screen/screen";

//仓库
const screenstore = useScreenStore();
let timer: any = ref(null);
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const autoScroll = (Id: any) => {
  // flag 为true时停止滚动
  let flag = ref(false);
  let element = document.querySelector(Id);
  function roll() {
    let h: any = ref(-1);
    timer.value = setInterval(function () {
      flag.value = true;
      //获取当前滚动条高度
      let current = document.querySelector(Id)?.scrollTop;
      if (current == h.value) {
        //滚动到底端,返回顶端
        h.value = 0;
        element!.scrollTop = h.value + 1;
      } else {
        //以25ms/3.5px的速度滚动
        h.value = current;
        element!.scrollTop = h.value + 1;
      }
    }, 25);
  }
  element.onmouseenter = () => {
    // console.log("鼠标进入");
    if (flag.value) {
      flag.value = false;
      clearInterval(timer.value);
      timer.value = null;
    }
  };
  element.onmouseleave = () => {
    // console.log("鼠标移除");
    if (!flag.value && !timer.value) {
      roll();
    }
  };
  roll();
};
onMounted(async () => {
  await screenstore.getSummaryInfo();
  //自动滚动效果
  autoScroll(".Mylist");
});
//清除定时器
onBeforeUnmount(() => {
  clearInterval(timer.value);
  timer.value = null;
  console.log(timer.value);
});
</script>

<style scoped lang="scss">
.inner {
  height: 100%;
  max-width: 860px;
  .title {
    font-size: 1.5rem;
    margin: 1rem;
    color: white;
  }
  .content {
    margin-top: 50px;
    .head {
      display: flex;
      justify-content: space-around;
      color: #4c9bfd;
      margin-bottom: 1rem;
      align-items: center;
      font-size: 1.2rem;
      div {
        text-align: center;
      }
      .col1 {
        width: 120px;
        // background-color: red;
      }
      .col2 {
        width: 110px;
        // background-color: blue;
      }
      .col3 {
        width: 100px;
        // background-color: green;
      }
    }
    .Mylist {
      width: 100%;
      height: 23rem;
      // overflow: auto;
      overflow: hidden;
      .row {
        color: #4c9bfd;
        padding: 10px;
        margin-bottom: 1rem;
        text-align: left;

        .list {
          display: flex;
          justify-content: space-around;
          align-items: center;
          div {
            overflow: hidden; /* 超出部分隐藏 */
            white-space: nowrap; /* 防止文本换行 */
            text-overflow: ellipsis; /* 显示三个点 */
          }
          .col1 {
            width: 120px;
            // background-color: red;
          }
          .col2 {
            width: 100px;
            // background-color: blue;
          }
          .col3 {
            width: 100px;
            // background-color: green;
          }
        }
      }
    }
    .Mylist::-webkit-scrollbar {
      width: 0;
    }
    .row:hover {
      opacity: 0.7;
      color: white;
      border: 1px solid white;
      border-right: transparent;
      border-radius: 10px;
    }
  }
}
</style>
