<!-- 冰箱内部页面模板 -->
<template>
  <div class="leftContainer">
    <div class="header">
      <!-- <el-progress type="dashboard" :percentage="percentage" :color="colors">
        <template #default="{ percentage }">
          <span class="percentage-value">{{ percentage }}%</span>
          <span class="percentage-label">{{ perlabel }}</span>
        </template>
      </el-progress> -->
      <PercentageComp
        :percentage="percentage"
        :perlabel="perlabel"
        :colors="colors"
      />
    </div>

    <el-tabs v-model="activeName" class="demo-tabs">
      <el-tab-pane label="食物情况" name="first">
        <!-- 食物状况 -->
        <FoodStatus />
      </el-tab-pane>
      <el-tab-pane label="即将过期" name="second">
        <ReadyExpire />
      </el-tab-pane>
      <el-tab-pane label="到期列表" name="third">
        <ExpireFood />
      </el-tab-pane>
    </el-tabs>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
// 引入食物状态
import FoodStatus from "./modules/FoodStatus.vue";
// 引入准备过期
import ReadyExpire from "./modules/ReadyExpire.vue";
// 引入过期
import ExpireFood from "./modules/ExpireFood.vue";
//引入进度条
import PercentageComp from "./modules/PercentageComp.vue";

defineProps(["perlabel", "percentage"]);
const activeName = ref("first");
const colors = [
  { color: "rgb(32,160,255)", percentage: 50 },
  { color: "#e6a23c", percentage: 70 },
  { color: "#f47920", percentage: 90 },
  { color: "#f56c6c", percentage: 100 },
];
</script>

<style scoped lang="scss">
.leftContainer {
  text-align: center;
  margin-top: 1rem;
  border-right: 1px solid #ccc;
  .header {
    display: flex;
    align-items: center;
    justify-content: center;
    .percentage-value {
      display: block;
      margin-top: 10px;
      font-size: 28px;
    }
    .percentage-label {
      display: block;
      margin-top: 10px;
      font-size: 12px;
    }
    .demo-progress .el-progress--line {
      margin-bottom: 15px;
      max-width: 600px;
    }
    .demo-progress .el-progress--circle {
      margin-right: 15px;
    }
  }
}
</style>
