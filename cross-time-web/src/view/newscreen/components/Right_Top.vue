<!-- 设备在线情况 -->
<template>
  <div class="rightTopbox">
    <!-- 设备总览内容 -->
    <div class="list">
      <!-- 注册设备数 -->
      <div class="item">
        <div class="item_top">
          <img width="100%" src="../images/left_top_lan.png" />
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '1.7rem',
            }"
            :value="registerEquipValue"
          />
        </div>
        <div class="item_text">注册设备</div>
      </div>
      <div class="item">
        <div class="item_top">
          <img width="100%" src="../images/left_top_lv.png" />
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '1.7rem',
            }"
            :value="onlineEquipValue"
          />
        </div>
        <div class="item_text">在线设备</div>
      </div>
      <div class="item">
        <div class="item_top">
          <img width="100%" src="../images/left_top_huang.png" />
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '1.7rem',
            }"
            :value="offLineEquipValue"
          />
        </div>
        <div class="item_text">离线设备</div>
      </div>
      <div class="item">
        <div class="item_top">
          <img width="100%" src="../images/left_top_hong.png" />
          <el-statistic
            :value-style="{
              color: 'white',
              fontSize: '1.7rem',
            }"
            :value="busyEquipValue"
          />
        </div>
        <div class="item_text">被控设备</div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { useTransition } from "@vueuse/core";
import { ref, onMounted, watch } from "vue";
import useScreenStore from "@/store/modules/screen/screen";
const screenstore = useScreenStore();
//注册设备数
let registerEquip = ref(0);
let registerEquipValue = useTransition(registerEquip, {
  duration: 1400,
});
//在线设备数
let onlineEquip = ref(0);
let onlineEquipValue = useTransition(onlineEquip, {
  duration: 1400,
});

//离线设备数
let offLineEquip = ref(0);
let offLineEquipValue = useTransition(offLineEquip, {
  duration: 1400,
});

//繁忙设备数
let busyEquip = ref(0);
let busyEquipValue = useTransition(busyEquip, {
  duration: 1400,
});

//这里监听仓库注册设备值的变化，然后来获取数据
watch(
  () => screenstore.allEquipmentList,
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  (newVal, oldVal) => {
    // console.log(newVal, oldVal);卡了许久的终于解决了！！！
    registerEquip.value = screenstore.getRegisterCount; //所有注册设备
    onlineEquip.value = screenstore.getCurrentOnlineEquipment; //在线设备
    offLineEquip.value = screenstore.getOfflineEquipment; //离线设备
    busyEquip.value = screenstore.getBusyOnlineEquipment; //繁忙设备
  },
);
onMounted(async () => {
  // console.log(registerEquipValue.value, screenstore.vnodeFor[0].count);
});
</script>

<style lang="scss" scoped>
.rightTopbox {
  height: 100%;
  width: 100%;
  .list {
    height: 100%;
    display: flex;
    justify-content: space-between;
    align-items: center;
    .item {
      text-align: center;
      margin: 5px;
      .item_top {
        position: relative;
        .el-statistic {
          position: absolute;
          left: 50%;
          top: 50%;
          color: white;
          transform: translate(-50%, -50%);
        }
      }
      .item_text {
        margin-top: 20px;
        font-size: 20px;
        color: rgb(0, 206, 215);
      }
    }
  }
}
</style>
