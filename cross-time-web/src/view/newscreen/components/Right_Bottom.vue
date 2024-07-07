<template>
  <!--生成实时天气信息卡片组件 -->
  <div class="hxp-weather">
    <div class="hxp-weather-header">
      <el-icon size="30"><Location /></el-icon>
      <div class="city">
        {{ screenStore.weatherData.current.city }}
      </div>
    </div>
    <div class="tempArea">
      <div class="left">
        <!-- 温度 -->
        <div class="top">
          {{ screenStore.weatherData.current.temp }}
          <span>℃</span>
        </div>
        <!-- 天气 -->
        <div class="bottom">
          {{ screenStore.weatherData.current.weather }}
        </div>
      </div>
      <div class="right">
        <!-- 风 -->
        <div class="top">
          <svg
            t="1718718083884"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="11101"
            width="32"
            height="32"
          >
            <path
              d="M660.48 665.6a122.88 122.88 0 1 1-118.92736 153.9072 20.48 20.48 0 1 1 39.64928-10.32192A81.96096 81.96096 0 0 0 742.4 788.48a81.92 81.92 0 0 0-81.92-81.92H97.28a20.48 20.48 0 0 1 0-40.96h563.2z m133.12-143.36a112.64 112.64 0 1 0-103.424-157.36832 20.48 20.48 0 1 1-37.56032-16.30208 153.6 153.6 0 1 1 156.99968 213.8112 20.48 20.48 0 0 1-5.77536 0.8192H97.28a20.48 20.48 0 0 1 0-40.96h696.32z m-348.16-184.32a92.16 92.16 0 1 0-86.56896-123.88352 20.48 20.48 0 0 1-38.44096-14.09024 133.16096 133.16096 0 1 1 141.70112 177.88928 20.45952 20.45952 0 0 1-6.4512 1.04448H97.28a20.48 20.48 0 0 1 0-40.96h348.16z"
              fill="#3973DC"
              p-id="11102"
            ></path>
          </svg>
          <div>{{ screenStore.weatherData.current.wind }}</div>
          <span>{{ screenStore.weatherData.current.windSpeed }}</span>
        </div>
        <!-- 湿度 -->
        <div class="center">
          <svg
            t="1718718055110"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="9941"
            width="32"
            height="32"
          >
            <path
              d="M267.815006 641.848567a248.46782 248.46782 0 0 1 86.998894-83.930602v-434.03563a123.882335 123.882335 0 0 1 247.76467 0v434.03563a249.298816 249.298816 0 1 1-372.605846 215.995051 253.965178 253.965178 0 0 1 37.842282-132.064449z m211.392611 339.877385a207.812936 207.812936 0 0 0 94.158245-393.125055l-11.250408-5.625205V123.882335a83.099605 83.099605 0 1 0-165.815674 0v458.645898l-11.250408 5.625204a208.13255 208.13255 0 0 0 94.158245 393.636438z"
              fill="#00A29F"
              p-id="9942"
            ></path>
            <path
              d="M442.260254 665.947452V262.083371a36.88344 36.88344 0 1 1 73.702958 0v403.352699a110.778166 110.778166 0 1 1-73.702958 0.511382zM274.399052 133.598596A355.985925 355.985925 0 0 0 232.40179 40.974498a355.985925 355.985925 0 0 0-41.997262 92.624098 41.741571 41.741571 0 0 0 41.997262 41.48588 41.102343 41.102343 0 0 0 41.997262-41.48588zM190.46845 501.665925a530.559018 530.559018 0 0 0-62.963931-138.201036A530.559018 530.559018 0 0 0 64.540587 501.665925a62.963932 62.963932 0 0 0 125.927863 0z m704.301117 95.88416a121.453269 121.453269 0 0 0 121.836805-120.30266c0-66.543607-121.836806-267.708575-121.836805-267.708574s-121.836806 201.164968-121.836806 267.708574a120.814042 120.814042 0 0 0 121.836806 120.110891z"
              fill="#00A29F"
              p-id="9943"
            ></path>
          </svg>
          <div>相对湿度</div>
          <span>{{ screenStore.weatherData.current.humidity }}</span>
        </div>
        <!-- 空气质量 -->
        <div class="bottom">
          <svg
            t="1718718008112"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="8930"
            width="32"
            height="32"
          >
            <path
              d="M346.636205 784.375582c-0.274687 3.883654-103.85711-56.517944-141.358249-128.414144-41.000231-72.813233-15.923405-157.116841 160.320116-222.268414 176.032223-74.896629 274.505464-161.710456 333.48714-221.034435 58.284394-60.731223 77.072997-93.972597 77.736472-94.213477-1.360758 1.001552 133.700815 222.196573 151.073724 433.033775 6.123411 105.412262-24.345736 209.763809-120.980686 270.068209-94.509294 58.715441-255.171713 73.387963-460.278517-37.171514z"
              fill="#85DF00"
              p-id="8931"
            ></path>
            <path
              d="M747.954175 886.842352c-121.914622 36.127703-277.539701 17.077091-436.625838-73.971145-8.012414-4.166793-57.249034-31.204464-99.386048-73.176664-64.450064-64.200733-82.249794-125.984219-65.772789-178.994619 21.129783-67.501205 92.687907-114.379742 202.064116-159.18756 167.753574-70.594605 263.091155-153.017663 320.585295-209.932847 56.87715-58.199875 75.902407-90.887649 76.946218-91.720162a38.506917 38.506917 0 0 1 31.969362-17.550398c12.927201-0.097197 25.000759 6.283997 31.622833 17.127802 2.687708 16.734788 279.141338 414.17333 114.844597 664.675361-39.267589 57.257486-99.96923 100.265047-176.247746 122.730232z m21.784806-698.237908c-50.373403 57.743471-152.852851 175.820925-386.138334 277.057942-84.079633 35.121925-135.374294 68.397108-145.740566 110.128429-20.005679 71.579253 91.470831 156.829476 141.861138 178.88897 0.621216 0.295817 1.233979 0.608538 1.829839 0.925484 132.475288 71.32147 261.033114 87.870316 346.139655 62.480769 43.628776-12.87649 77.935092-36.639044 103.142923-71.046783 108.93671-149.501667 4.678134-420.076992-61.094655-558.434811z"
              fill="#85DF00"
              p-id="8932"
            ></path>
            <path
              d="M103.263363 941.598072c-23.234309-1.141008-44.621876-11.291756-52.477929-26.006537-10.264849-19.316848 5.87408-38.697085 35.058536-44.203506-0.088745-0.37611 174.337614-29.5986 344.166133-153.359966 158.308561-109.033907 323.087061-333.360362 309.758394-596.77269-0.557826-19.912708 14.634488-36.867246 34.65707-38.78583 19.997227-1.90168 38.122355 11.955231 39.766252 31.914425 27.346165 280.595068-144.633365 530.057513-314.994355 652.03975-181.944336 138.011291-368.811912 172.047146-374.292977 173.581168a86.788471 86.788471 0 0 1-21.641124 1.593186z"
              fill="#69B000"
              p-id="8933"
            ></path>
            <path
              d="M668.13008 576.754333c-16.122024 4.885206-33.676648 3.211727-45.352966-5.430354-1.867873-1.39034-19.101324-14.364027-38.642148-41.845422-25.309254-35.544521-43.147017-78.484466-54.717686-126.846314-4.513322-19.28304 9.91832-39.16194 32.024299-45.179702 22.105979-6.000858 44.626102 3.828717 50.504408 22.739872 28.216712 95.231932 78.733798 132.517548 79.519826 132.627423 17.905378 12.005943 17.91383 35.29519-1.293143 51.962362-6.360065 5.510647-14.017498 9.550662-22.04259 11.972135zM656.690416 709.783222c-74.283865 20.200073-134.486843 19.993001-138.670541 20.081745-28.499851 0.04226-47.719502-16.96299-43.476641-36.972894 4.251312-20.014131 29.260524-35.738915 56.395391-36.072766 0.629668 1.246657 148.022582-4.466836 232.452969-77.647727 19.933837-16.831985 50.136749-21.400244 68.726733-10.07468 18.560401 11.321338 18.061739 35.109248-2.366536 53.0104-50.022648 43.928819-115.56301 72.225825-173.061375 87.675922z"
              fill="#69B000"
              p-id="8934"
            ></path>
          </svg>
          <div>空气质量</div>
          <span>{{ screenStore.weatherData.current.air }}</span>
        </div>
      </div>
    </div>
    <!-- 指数相关 -->
    <div class="index">
      <div
        class="list_item"
        v-for="(item, index) in screenStore.weatherData.living"
        :key="index"
      >
        <el-tooltip
          style="z-index: -99"
          effect="light"
          :hide-after="0"
          placement="top"
        >
          <template #content>
            {{ item.name }}：{{ item.index }}
            <br />
            {{ item.tips }}
          </template>
          <div class="box-item">
            <div>{{ item.name }}</div>
            <div>{{ item.index }}</div>
            <div>{{ item.tips }}</div>
          </div>
        </el-tooltip>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from "vue";
import UseScreenStore from "@/store/modules/screen/screen";
const screenStore = UseScreenStore();
const init = async () => {
  await screenStore.getWeatherData();
};

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
    if (flag.value) {
      flag.value = false;
      clearInterval(timer.value);
      timer.value = null;
    }
  };
  element.onmouseleave = () => {
    if (!flag.value && !timer.value) {
      roll();
    }
  };
  roll();
  // }, 2000);
};
let timer = ref();
onMounted(() => {
  init();
  timer.value = setInterval(() => {
    init();
  }, 60000);
  autoScroll(".index");
});
onBeforeUnmount(() => {
  clearInterval(timer.value);
  console.log("%c关闭了定时器", "color:red");
});
</script>

<style scoped lang="scss">
.hxp-weather-header {
  width: 100%;
  height: 1.2rem;
  // background-color: red;
  color: white;
  display: flex;
  align-items: center;
  .city {
    margin-left: 5px;
    font-size: 1.2rem;
  }
}
.tempArea {
  margin-top: 2rem;
  display: flex;
  align-items: center;
  // 天气左半部分
  .left {
    color: white;
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    .top {
      // background-color: red;
      font-size: 3.4rem;
      position: relative;
      text-align: center;
      span {
        font-size: 1.2rem;
        position: absolute;
        top: -5px;
      }
    }
    .bottom {
      margin-top: 20px;
      font-size: 1.5rem;
    }
  }
  //天气右半部分
  .right {
    color: white;
    font-size: 1.2rem;
    // background-color: blue;
    flex: 1;
    .top {
      display: flex;
      align-items: center;
      div {
        margin-left: 10px;
      }
      span {
        margin-left: 10px;
      }
    }
    .center {
      margin-top: 10px;
      display: flex;
      align-items: center;
      div {
        margin-left: 10px;
      }
      span {
        margin-left: 10px;
      }
    }
    .bottom {
      margin-top: 10px;
      display: flex;
      align-items: center;
      div {
        margin-left: 10px;
      }
      span {
        margin-left: 10px;
      }
    }
  }
}
.index {
  // border: 1px solid #ffffff;
  // opacity: 0.1;
  width: 100%;
  height: 10rem;
  // background-color: white;
  // overflow-y: auto;
  overflow: hidden;
  .list_item {
    overflow: hidden;
    // border: 1px solid #ccc;
    // border-radius: 10px;
    width: 100%;
    // background-color: red;
    .box-item {
      display: flex;
      align-items: center;
      margin-top: 0.5rem;
      text-align: left;
      flex: 1;
      div:nth-child(1) {
        // flex: 1;
        padding: 10px;
        color: rgb(0, 123, 255);
        max-width: 100px;
        text-align: left;
        overflow: hidden;
        -webkit-line-clamp: 1; //设置行数
        text-overflow: ellipsis;
        white-space: nowrap;
        // background-color: blue;
        width: 100px;
      }
      div:nth-child(2) {
        color: rgb(249, 249, 249);
        text-align: left;
        // flex: 0.5;
        // max-width: 100px;
        padding: 10px;
        // background-color: red;
        width: 80px;
      }
      div:hover {
        cursor: pointer;
      }
      div:nth-child(3) {
        // flex: 2;
        padding: 10px;
        max-width: 100px;
        color: rgb(0, 123, 255);
        overflow: hidden;
        -webkit-line-clamp: 1; //设置行数
        text-overflow: ellipsis;
        white-space: nowrap;
        // background-color: green;
      }
    }

    // justify-content: space-around;
    // justify-content: space-between;
  }
  .list_item:hover {
    border: 1px solid #ccc;
    border-radius: 10px;
  }
}
.index::-webkit-scrollbar {
  width: 0;
}
</style>
