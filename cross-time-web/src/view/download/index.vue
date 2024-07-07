<template>
  <div class="downloadContainer">
    <div class="header">
      <div class="left">
        <img src="@/assets/images/logo.jpg" />
      </div>
      <div class="right">
        <div class="text">{{ setting.title }}最新版</div>
        <div class="des"><span>跨越时空，远程连接</span></div>
        <div class="btn">
          <button @click="download(dataArr[0].url)">点击下载</button>
        </div>
        <div class="version">当前版本:{{ dataArr[0].version }}</div>
      </div>
    </div>
    <div class="content">
      <div class="history_version">历史版本</div>
      <div class="list">
        <el-card class="list_item" v-for="item in dataArr" :key="item.id">
          <div class="container">
            <div class="left">
              <img width="100rem" src="@/assets/images/logo.jpg" />
            </div>
            <div class="center">
              <div class="text">{{ setting.title }}{{ item.versionTitle }}</div>
              <div class="version">当前版本:{{ item.version }}</div>
              <el-tooltip
                class="box-item"
                effect="light"
                placement="bottom-start"
              >
                <template #content>
                  <p>被控端安装程序</p>
                  <p>更新内容：</p>
                  <ul>
                    <li v-for="items in item.updateLogList" :key="items.id">
                      {{ items.title }}
                    </li>
                  </ul>
                </template>
                <div class="more">查看更多</div>
              </el-tooltip>
            </div>
            <div class="right">
              <button @click="download(item.url)">点击下载</button>
            </div>
          </div>
        </el-card>
      </div>
    </div>
    <div class="footer">
      <el-divider></el-divider>
      <div class="publish">
        {{ setting.title }}-下载中心 Copyright © {{ year }} CTCtrl All Rights
        Reserved
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import setting from "@/utils/setting";
import { ref } from "vue";
const year = ref(new Date().getFullYear());
let currentUrl = import.meta.env.VITE_APP_DOWNLOAD;
const dataArr = ref([
  {
    id: 2,
    versionTitle: "-被控端",
    version: "V2.0",
    url: currentUrl + "/CTCtrl-beCtrl-V2.0.exe",
    updateLogList: [
      {
        title: "1.修复软件安装PATH环境变量不随安装位置变化的问题",
        id: 1,
      },
      {
        title: "2.添加本地加载文件，被控端使用更智能",
        id: 2,
      },
      {
        title: "3.新增文件预览、关机重启等功能",
        id: 3,
      },
    ],
  },
  {
    id: 1,
    versionTitle: "-被控端",
    version: "V1.0",
    url: currentUrl + "/CTCtrl-beCtrl-V1.0.exe",
    updateLogList: [
      {
        title: "1.支持用户登录",
        id: 1,
      },
      {
        title: "2.支持设备被其他用户控制（这里没有加限制，全网公开）",
        id: 2,
      },
      {
        title: "3.适配win托盘功能，支持Toast通知",
        id: 3,
      },
      {
        title: "4.当前为软件测试阶段，请谨慎安装",
        id: 4,
      },
    ],
  },
]);
//下载
const download = (item: any) => {
  // console.log(item);
  const a = document.createElement("a");
  a.href = item;
  a.download = item;
  a.click();
};
</script>
<style scoped lang="scss">
@media screen and (max-width: 700px) {
  .content {
    width: 100% !important;
    .container {
      width: 50% !important;
    }
  }
  .list {
    // background-color: red;
    display: grid !important;
    grid-template-columns: repeat(1, 1fr) !important;
    grid-column-gap: 5rem !important;
    grid-row-gap: 2rem !important;
    .list_item {
      width: 100% !important;
    }
  }
}
.downloadContainer {
  background-color: white;
  .header {
    height: 25rem;
    display: flex;
    background-image: linear-gradient(to top, #a8edea 0%, #fed6e3 100%);
    position: relative;
    .left {
      height: 100%;
      display: flex;
      flex: 1;
      justify-content: center;
      align-items: center;
      user-select: none;
      pointer-events: none;
    }
    .right {
      flex: 1;
      height: 100%;

      .text {
        font-size: 2rem;
        color: white;
      }
      .des {
        font-size: 1.4rem;
        color: white;
        display: flex;
        align-items: start;
      }
      .btn {
        width: 18rem;
        height: 4rem;
        button {
          border: 1px solid white;
          border-radius: 2rem;
          height: 100%;
          background-color: rgb(197, 155, 97);
          opacity: 0.8;
          color: white;
          width: 100%;
        }
        button:hover {
          opacity: 1;
        }
      }
      .version {
        color: white;
      }
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: space-around;
    }
  }
  .content {
    min-width: 58rem;
    margin: 1rem auto;
    width: 70%;
    .history_version {
      font-size: 1.7rem;
      margin: 1rem;
    }
    .list {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      grid-column-gap: 5rem;
      grid-row-gap: 2rem;
      .list_item {
        .container {
          display: flex;
          .left {
            flex: 1;
            display: flex;
            justify-content: start;
            align-items: center;
          }
          .center {
            .text {
              font-size: 1rem;
              font-weight: 600;
              min-width: 12rem;
              text-align: center;
            }
            .version {
              font-size: 0.8rem;
              color: grey;
            }
            .text:hover {
              color: rgb(0, 102, 255);
            }
            .more {
              cursor: pointer;
              font-size: 0.8rem;
            }
            flex: 1;
            display: flex;
            flex-direction: column;
            justify-content: space-around;
            align-items: center;
          }
          .right {
            flex: 1;
            display: flex;
            justify-content: end;
            align-items: center;
            button {
              border: 1px solid grey;
              width: 6rem;
              height: 2rem;
              background-color: rgb(255, 255, 255);
              border-radius: 1rem;
              color: rgb(32, 198, 127);
              font-size: 0.9rem;
            }
            button:hover {
              background-color: rgb(32, 198, 127);
              color: white;
              cursor: pointer;
            }
          }
        }
      }
    }
  }
  .footer {
    // height: 30rem;
    margin-top: 14%;
    // background-color: red;
    .publish {
      text-align: center;
      color: rgb(158, 158, 158);
      // height: 5rem;
      line-height: 5rem;
    }
  }
}
</style>
