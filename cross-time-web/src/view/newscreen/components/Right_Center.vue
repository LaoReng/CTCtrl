<template>
  <div class="CenterBox">
    <div class="charts" ref="charts"></div>
  </div>
</template>

<script setup>
import * as echarts from "echarts";
import { ref, onMounted } from "vue";
import useScreenStore from "@/store/modules/screen/screen";
//仓库
const screenstore = useScreenStore();
let data = ref([]);
let charts = ref();
onMounted(async () => {
  let mycharts = echarts.init(charts.value);
  //这里多发送了一次获取在线列表的请求
  await screenstore.getSummaryInfo();
  // console.log(screenstore.getHistoryList);
  data.value = screenstore.dataSummaryInfo.weekPerDayTotal;
  // data.value = screenstore.dataSummaryInfo.weekPerDayTotal;
  let option = {
    color: ["#80FFA5", "#00DDFF", "#37A2FF", "#FF0087", "#FFBF00"],
    title: {
      text: "最近一周内访问统计",
      textStyle: {
        color: "white",
      },
    },
    tooltip: {
      trigger: "axis",
      axisPointer: {
        type: "cross",
        label: {
          backgroundColor: "#6a7985",
        },
      },
    },
    // legend: {
    //   textStyle: {
    //     color: "white",
    //   },
    // },
    grid: {
      left: "3%",
      right: "4%",
      bottom: "3%",
      containLabel: true,
    },
    xAxis: [
      {
        type: "category",
        boundaryGap: false,
        data: ["周一", "周二", "周三", "周四", "周五", "周六", "周日"],
        axisLine: {
          lineStyle: {
            color: "white",
          },
        },
      },
    ],
    yAxis: [
      {
        type: "value",
        axisLine: {
          lineStyle: {
            color: "white",
          },
        },
      },
    ],
    series: [
      {
        type: "line",
        stack: "Total",
        smooth: true,
        lineStyle: {
          width: 0,
        },
        showSymbol: false,
        areaStyle: {
          opacity: 0.8,
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            {
              offset: 0,
              color: "rgb(128, 255, 165)",
            },
            {
              offset: 1,
              color: "rgb(1, 191, 236)",
            },
          ]),
        },
        emphasis: {
          focus: "series",
        },
        data: data.value,
      },
    ],
    emphasis: {
      focus: "series",
    },
    // data: [220, 402, 231, 134, 190, 230, 120],
  };
  mycharts.setOption(option);
});
</script>

<style lang="scss" scoped>
.CenterBox {
  margin-top: 40px;
  background-size: cover;

  .charts {
    margin-top: 10px;
    height: 260px;
  }
}
</style>
