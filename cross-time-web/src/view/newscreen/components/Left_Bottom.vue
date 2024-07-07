<template>
  <div class="inner">
    <h3>设备分布统计</h3>
    <div class="chart">
      <div class="pie" ref="charts"></div>
      <div class="data">
        <div class="item">
          <h4>17</h4>
          <span>
            <i class="icon-dot" style="color: #ed3f35"></i>
            设备总数
          </span>
        </div>
        <div class="item">
          <h4>12</h4>
          <span>
            <i class="icon-dot" style="color: #eacf19"></i>
            本月新增
          </span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import * as echarts from "echarts";
import { ref, onMounted } from "vue";
let charts = ref();
onMounted(() => {
  let mycharts = echarts.init(charts.value);
  let option = {
    // backgroundColor: "#2c343c",
    // title: {
    //   text: "Customized Pie",
    //   left: "center",
    //   top: 20,
    //   textStyle: {
    //     color: "white",
    //   },
    // },
    tooltip: {
      trigger: "item",
    },
    visualMap: {
      show: false,
      min: 80,
      max: 600,
      inRange: {
        colorLightness: [0, 1],
      },
    },
    series: [
      {
        name: "地区",
        type: "pie",
        radius: "75%",
        center: ["50%", "50%"],
        data: [
          {
            value: 335,
            name: "黑龙江",
            itemStyle: {
              color: "#FF6384", // 自定义颜色
            },
          },
          {
            value: 310,
            name: "云南",
            itemStyle: {
              color: "#00FFFF", // 自定义颜色
            },
          },
          {
            value: 274,
            name: "上海",
            itemStyle: {
              color: "#FFF0F5", // 自定义颜色
            },
          },
          {
            value: 235,
            name: "吉林",
            itemStyle: {
              color: "#00EE00", // 自定义颜色
            },
          },
          {
            value: 235,
            name: "青海",
            itemStyle: {
              color: "#ccc", // 自定义颜色
            },
          },
          {
            value: 400,
            name: "内蒙古",
            itemStyle: {
              color: "#FFF68F", // 自定义颜色
            },
          },
        ].sort(function (a, b) {
          return a.value - b.value;
        }),
        roseType: "radius",
        label: {
          show: true,
          // position: "outer",
          // formatter: "{b}: {d}%",
          // color: "red",
          color: function (params: { data: { itemStyle: { color: any } } }) {
            return params.data.itemStyle.color;
          },
        },
        emphasis: {
          label: {
            show: true,
            fontSize: "14",
            fontWeight: "bold",
          },
        },
        labelLine: {
          lineStyle: {
            color: "white",
          },
          smooth: 0.2,
          length: 10,
          length2: 20,
        },
        // itemStyle: {
        //   color: "red",
        //   shadowBlur: 200,
        //   shadowColor: "rgba(0, 0, 0, 0.5)",
        // },
        animationType: "scale",
        animationEasing: "elasticOut",
        animationDelay: function () {
          return Math.random() * 200;
        },
      },
    ],
  };
  mycharts.setOption(option);
});
</script>

<style scoped lang="scss">
.inner {
  height: 100%;
  h3 {
    margin-left: 20px;
    font-size: 1.4rem;
  }
  .chart {
    display: flex;
    margin-top: 10px;
    .pie {
      flex: 2;
      height: 200px;
    }
    .data {
      display: flex;
      flex-direction: column;
      justify-content: space-around;
      // font-size: 1.4rem;
      align-items: center;
      width: 4rem;
      .item {
        display: flex;
        flex-direction: column;
        align-items: center;
        span {
          margin-top: 10px;
        }
      }
    }
  }
}
</style>
