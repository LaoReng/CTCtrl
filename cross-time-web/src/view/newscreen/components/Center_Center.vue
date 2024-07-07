<template>
  <div class="legend">
    <div class="lan">
      <div class="left"></div>
      <div class="right">用户所在位置</div>
    </div>
    <div class="lv">
      <div class="left"></div>
      <div class="right">设备空闲在线</div>
    </div>
    <div class="hui">
      <div class="left"></div>
      <div class="right">设备离线</div>
    </div>
    <div class="yellow">
      <div class="left"></div>
      <div class="right">设备正被控制</div>
    </div>
  </div>
  <div class="map" ref="map"></div>
</template>

<script setup lang="ts">
// import axios from "axios";
import { ref, onMounted } from "vue";
import * as echarts from "echarts";
import useScreenStore from "@/store/modules/screen/screen";
import china from "./china.json";
//引入仓库
const screenstore = useScreenStore();
const mapData: any = china;
let map = ref();
onMounted(async () => {
  await screenstore.getRegisterStatus();
  await screenstore.getCtrlToBeCtrl();
  // console.log(screenstore.getEquipmentDirectMap);
  let points: any = ref([...screenstore.points]);
  //这里是指向的区域，一个数组里面代表指向区域
  const linesData = ref([...screenstore.getEquipmentDirectMap]);
  const option = {
    backgroundColor: "#0E2152", // 背景颜色
    geo: {
      // 地图配置
      map: "china",
      zoom: 1.2, //初始化放大倍率
      roam: true, //是否可以拖拽
      label: {
        // 图形上的文本标签
        // normal: {
        // 通常状态下的样式
        show: true,
        // textStyle: {
        color: "#fff",
        // },
        // },
        label: {
          // 鼠标放上去高亮的样式
          textStyle: {
            color: "blue",
          },
        },
      },
      itemStyle: {
        // 地图区域的样式设置
        // normal: {
        // 通常状态下的样式
        borderColor: "#5089EC",
        borderWidth: 1,
        areaColor: {
          //地图区域的颜色
          type: "radial", // 径向渐变
          x: 0.5, // 圆心
          y: 0.5, // 圆心
          r: 0.8, // 半径
          colorStops: [
            {
              // 0% 处的颜色
              offset: 0,
              color: "rgba(0, 102, 154, 0)",
            },
            {
              // 100% 处的颜色
              offset: 1,
              color: "rgba(0, 102, 154, .4)",
            },
          ],
          // },
        },
      },
      // 鼠标放上去高亮的样式
      emphasis: {
        itemStyle: {
          areaColor: "#2386AD",
          borderWidth: 0,
        },
      },
    },
    series: [
      {
        name: "在线",
        // 散点系列数据
        type: "effectScatter", // 带有涟漪特效动画的散点（气泡）图
        coordinateSystem: "geo", //该系列使用的坐标系:地理坐标系

        // 特效类型,目前只支持涟漪特效'ripple'，意为“涟漪”
        effectType: "ripple",
        // 配置何时显示特效。可选'render'和'emphasis' 。
        showEffectOn: "render",
        rippleEffect: {
          // 涟漪特效相关配置。
          period: 3, // 动画的周期，秒数。
          scale: 4, // 动画中波纹的最大缩放比例。
          // 波纹的绘制方式，可选 'stroke' 和 'fill'。
          brushType: "stroke",
        },
        zlevel: 1, // 所有图形的 zlevel 值。
        data: points.value,
      },
      {
        // 线条系列数据
        type: "lines",
        zlevel: 2,
        // symbol: ["none", "arrow"], // 标记的图形: 箭头
        // symbolSize: 200, // 标记的大小
        effect: {
          // 线条特效的配置
          show: true,
          period: 4, // 特效动画的时间，单位s
          trailLength: 0.2, // 特效尾迹的长度。取值[0,1]值越大，尾迹越重
          symbol: "pin", // 特效图形的标记 可选'circle'等
          symbolSize: 10, // 特效标记的大小
        },
        lineStyle: {
          // 线条样式
          // normal: {
          color: "#93EBF8",
          width: 2.5, // 线条宽度
          opacity: 0.1, // 尾迹线条透明度
          curveness: 0.2, // 尾迹线条曲直度
          // },
        },
        //指向区域
        data: linesData.value,
      },
    ],
  };
  echarts.registerMap("china", mapData as any);
  let myEcharts = echarts.init(map.value);
  myEcharts.setOption(option);
});
</script>

<style scoped lang="scss">
.map {
  height: 100%;
  background-color: rgba(255, 255, 255, 0.05);
  display: flex;
  flex-direction: column;
}
.legend {
  position: absolute;
  color: white;
  // opacity: 0.6;
  border: 1px solid #ccc;
  padding: 10px;
  border-radius: 10px;
  // filter: blur(1px);
  font-family: Arial, sans-serif;
  box-shadow: 2px 2px 2px 1px rgba(0, 0, 0, 0.2);
  left: 25%;
  bottom: 7%;
  z-index: 9999;
  display: flex;
  flex-direction: column;
  align-items: start;
  .lan {
    display: flex;
    align-items: center;
    // width: 120px;
    .left {
      margin-right: 10px;
      width: 14px;
      border-radius: 10px;
      height: 14px;
      background-color: rgb(0, 238, 255);
    }
    margin-bottom: 10px;
  }
  .lv {
    display: flex;
    align-items: center;
    // width: 120px;
    .left {
      margin-right: 10px;
      width: 14px;
      height: 14px;
      border-radius: 10px;
      background-color: rgb(0, 255, 0);
    }
    margin-bottom: 10px;
  }
  .hui {
    display: flex;
    align-items: center;
    // width: 120px;
    .left {
      margin-right: 10px;
      width: 14px;
      height: 14px;
      border-radius: 10px;
      background-color: rgb(230, 230, 230);
    }
    margin-bottom: 10px;
  }
  .yellow {
    display: flex;
    align-items: center;
    // width: 120px;
    .left {
      margin-right: 10px;
      width: 14px;
      height: 14px;
      border-radius: 10px;
      background-color: rgb(238, 255, 0);
    }
  }
}
</style>
