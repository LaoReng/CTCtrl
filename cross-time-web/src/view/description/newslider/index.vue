<template>
  <el-aside width="300px">
    <div
      :style="{
        background: item.isRed ? 'red' : '',
        color: item.isRed ? 'white' : '',
      }"
      v-for="(item, index) in list"
      :key="item.id"
      @click="clickSliderItem(item, index)"
    >
      <span>{{ item.tagName }}</span>
    </div>
  </el-aside>
</template>

<script setup lang="ts">
import { ref, onMounted } from "vue";
const $emit = defineEmits(["child-click"]);
let list = ref([
  {
    tagName: "注册",
    isRed: true,
    id: 1,
  },
  {
    tagName: "登录",
    isRed: false,
    id: 2,
  },
  {
    tagName: "下载客户端",
    isRed: false,
    id: 3,
  },
  {
    tagName: "安装客户端",
    isRed: false,
    id: 4,
  },
  {
    tagName: "如何控制设备",
    isRed: false,
    id: 5,
  },
]);

//点击回调
const clickSliderItem = (item: any, index: any) => {
  for (let i = 0; i < list.value.length; i++) {
    list.value[i].isRed = false;
  }
  list.value[index].isRed = true;
  $emit("child-click", item);
};
onMounted(() => {
  //初始化
  $emit("child-click", list.value[0]);
});
</script>

<style scoped lang="scss">
.el-aside {
  height: calc(100vh - 14rem);
  span {
    margin-left: 1rem;
  }
  div {
    width: 100%;
    line-height: 4rem;
    height: 4rem;
    background-color: rgb(255, 255, 255);
    font-family: Arial;
    border-radius: 0.65rem;
    // margin: 0 0.8rem;
    margin-bottom: 1rem;
    font-weight: 600;
    overflow-y: auto;
  }
  div:hover {
    background-color: rgb(244, 230, 232);
  }
}
.el-aside::-webkit-scrollbar {
  display: none;
}
</style>
