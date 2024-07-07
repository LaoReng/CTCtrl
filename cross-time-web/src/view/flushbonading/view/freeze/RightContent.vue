<template>
  <div class="leftContainer">
    <div class="header">
      <!-- 进度条 -->
      <el-progress type="dashboard" :percentage="83" :color="colors">
        <template #default="{ percentage }">
          <span class="percentage-value">{{ percentage }}%</span>
          <span class="percentage-label">{{ "冷藏容量" }}</span>
        </template>
      </el-progress>
    </div>

    <!-- tabs -->
    <el-tabs v-model="activeName" class="demo-tabs">
      <el-tab-pane label="食物情况" name="first">
        <!-- 食物状况 -->
        <div>
          <div
            style="margin-bottom: 10px"
            v-for="(item, index) in food_status"
            :key="index"
          >
            <div
              style="
                margin-bottom: 10px;
                color: black;
                text-align: left;
                font-size: 12px;
              "
            >
              {{ item.foodType }}
            </div>
            <el-progress
              text-inside
              :percentage="item.per"
              :stroke-width="16"
              striped
              striped-flow
              :color="item.color"
            />
          </div>
          <!-- 食物状况列表 -->
          <div class="food_list">
            <el-table :data="tableData">
              <!-- 食物名字 -->
              <el-table-column prop="Name" label="NAME" />
              <!--fresh degree新鲜度-->
              <el-table-column prop="Fd" label="FD" />
              <!-- 数量 -->
              <el-table-column prop="Num" label="NUM" />
            </el-table>
          </div>
        </div>
      </el-tab-pane>
      <el-tab-pane label="即将过期" name="second">
        <div>
          <div
            style="margin-bottom: 10px"
            v-for="(item, index) in food_status"
            :key="index"
          >
            <div
              style="
                margin-bottom: 10px;
                color: black;
                text-align: left;
                font-size: 12px;
              "
            >
              {{ item.foodType }}
            </div>
            <el-progress
              text-inside
              :percentage="item.per"
              :stroke-width="16"
              striped
              striped-flow
              :color="item.color"
            />
          </div>
          <!-- 食物状况列表 -->
          <div class="food_list">
            <el-table :data="readyData">
              <!-- 食物名字 -->
              <el-table-column prop="Name" label="NAME" />
              <!--fresh degree新鲜度-->
              <el-table-column prop="Fd" label="FD" />
              <!-- 数量 -->
              <el-table-column prop="Num" label="NUM" />
            </el-table>
          </div>
        </div>
      </el-tab-pane>
      <el-tab-pane label="到期列表" name="third">
        <div>
          <div
            style="margin-bottom: 10px"
            v-for="(item, index) in food_status"
            :key="index"
          >
            <div
              style="
                margin-bottom: 10px;
                color: black;
                text-align: left;
                font-size: 12px;
              "
            >
              {{ item.foodType }}
            </div>
            <el-progress
              text-inside
              :percentage="item.per"
              :stroke-width="16"
              striped
              striped-flow
              :color="item.color"
            />
          </div>
          <!-- 食物状况列表 -->
          <div class="food_list">
            <el-table :data="expireData">
              <!-- 食物名字 -->
              <el-table-column prop="Name" label="NAME" />
              <!--fresh degree新鲜度-->
              <el-table-column prop="Fd" label="FD" />
              <!-- 数量 -->
              <el-table-column prop="Num" label="NUM" />
            </el-table>
          </div>
        </div>
      </el-tab-pane>
    </el-tabs>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
const activeName = ref("first");
const colors = [
  { color: "rgb(32,160,255)", percentage: 50 },
  { color: "#e6a23c", percentage: 70 },
  { color: "#f47920", percentage: 90 },
  { color: "#f56c6c", percentage: 100 },
];
const tableData = [
  {
    Name: "猪肉",
    Fd: "93%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "鸡肉",
    Fd: "42%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "冰淇淋",
    Fd: "74%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "火锅丸",
    Fd: "34%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "鱼",
    Fd: "72%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "帝王蟹",
    Fd: "25",
    Num: Math.floor(Math.random() * 100),
  },
];
const food_status = ref([
  {
    foodType: "正常食物",
    color: "rgb(0, 255, 0)",
    per: 70,
  },
  {
    foodType: "即将过期食物",
    color: "rgb(255, 204, 0)",
    per: 20,
  },
  {
    foodType: "过期食物",
    color: "rgb(255,0, 0)",
    per: 10,
  },
]);
const readyData = ref([
  {
    Name: "鱼",
    Fd: "12%",
    Num: Math.floor(Math.random() * 100),
  },
]);
const expireData = ref([
  {
    Name: "帝王蟹",
    Fd: "1",
    Num: Math.floor(Math.random() * 100),
  },
]);
</script>

<style scoped lang="scss">
.header {
  margin-top: 1.1rem;
  text-align: center;
}
</style>
