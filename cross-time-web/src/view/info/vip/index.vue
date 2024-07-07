<template>
  <div class="vipContainer">
    <div class="header">
      <!-- 左侧头像部分 -->
      <div class="left">
        <el-badge class="item">
          <img class="head" src="@/assets/images/vip-head.png" alt="" />
          <img
            class="img"
            width="120rem"
            :src="worldAllData + userStore.userInfo.avatar"
          />
        </el-badge>
        <div class="vip_grade">VIP{{ userStore.userInfo.vipgrade }}</div>
      </div>
      <div class="center">
        <div class="title">
          <span>当前特权状态</span>
          <img height="20rem" src="@/assets/images/vip-grade.png" />
        </div>
        <div class="content">
          <div class="counts">
            每日可控次数：{{ userStore.userInfo.equipCtrlCount }}
          </div>
          <div class="date">
            会员到期时间：{{ userStore.userInfo.expireTime }}
          </div>
          <div class="grade">
            当前&nbsp;VIP&nbsp;等级：vip{{ userStore.userInfo.vipgrade }}
          </div>
          <div class="balance">
            累计充值金额：{{ userStore.userInfo.accBalance }}
            元
          </div>
        </div>
      </div>
      <!-- 右侧开会员部分 -->
      <div class="right">
        <div class="toContinue" @click="toPay"><button>去续费</button></div>
        <div class="toContinue" @click="toPay"><button>去充值</button></div>
        <div class="toContinue" @click="toPay"><button>升级会员</button></div>
      </div>
    </div>
    <div class="table">
      <el-table
        :header-cell-style="tableCellStyle"
        :data="tableData"
        style="margin-top: 3rem"
        :scrollbar-always-on="true"
      >
        <el-table-column
          align="center"
          prop="date"
          label-class-name="my"
          label="特权名称"
          width="180"
          highlight-current-row="true"
        />
        <el-table-column align="center" prop="v1" label="V1青铜" width="180" />
        <el-table-column align="center" prop="v2" label="V2白银" width="180" />
        <el-table-column align="center" prop="v3" label="V3黄金" width="180" />
        <el-table-column align="center" prop="v4" label="V4铂金" width="180" />
        <el-table-column align="center" prop="v5" label="V5钻石" width="180" />
        <el-table-column align="center" prop="v6" label="V6星耀" width="180" />
        <el-table-column align="center" prop="v7" label="V7王者" width="180" />
        <el-table-column
          align="center"
          prop="v8"
          label="V8时空会员"
          width="180"
        />
      </el-table>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ElMessage } from "element-plus";
import useUserStore from "@/store/modules/user/user";
const userStore = useUserStore();
// console.log(userStore);
let worldAllData = import.meta.env.VITE_APP_IMAGE;
const tableData = [
  {
    date: "可控次数",
    v1: "10次",
    v2: "15次",
    v3: "20次",
    v4: "25次",
    v5: "30次",
    v6: "35次",
    v7: "40次",
    v8: "无限",
  },
  {
    date: "可控设备数量",
    v1: "3台",
    v2: "6台",
    v3: "9台",
    v4: "12台",
    v5: "15台",
    v6: "18台",
    v7: "21台",
    v8: "无限",
  },
  {
    date: "可控时间",
    v1: "10分钟",
    v2: "20分钟",
    v3: "30分钟",
    v4: "40分钟",
    v5: "50分钟",
    v6: "60分钟",
    v7: "70分钟",
    v8: "无限",
  },
];
const tableCellStyle = ({ columnIndex }: any) => {
  // return { backgroundColor: "#ff0000" }; // 强制设置为红色，仅用于测试
  const headerStyles: { [key: string]: string } = {}; // 定义一个空对象来存储样式
  switch (columnIndex) {
    case 0:
      headerStyles.color = "black";
      break;
    case 1:
      headerStyles.background =
        "linear-gradient(to bottom right,#C6FFDD, #FBD786)";
      headerStyles.color = "white";
      break;
    case 2:
      headerStyles.background =
        "linear-gradient(to bottom right,#D3CCE3, #E9E4F0)";
      headerStyles.color = "white";
      break;
    case 3:
      headerStyles.background =
        "linear-gradient(to bottom right,#F2994A, #f2c94c)";
      headerStyles.color = "white";
      break;
    case 4:
      headerStyles.background =
        "linear-gradient(to bottom right,#87CEEB, #00F5FF)";
      headerStyles.color = "white";
      break;
    case 5:
      headerStyles.background =
        "linear-gradient(to bottom right,#E0FFFF,#00FFFF)";
      headerStyles.color = "white";
      break;
    case 6:
      headerStyles.background =
        "linear-gradient(to bottom right,#9B30FF,#EED2EE)";
      headerStyles.color = "white";
      break;
    case 7:
      headerStyles.background =
        "linear-gradient(to bottom right,#FF6A6A, #FF3E96)";
      headerStyles.color = "white";
      break;
    case 8:
      headerStyles.background = "linear-gradient(to bottom right,blue, pink)";
      headerStyles.color = "white";
      break;
    default:
      headerStyles.backgroundColor = ""; // 默认颜色或不设置
  }
  return headerStyles;
};

const toPay = () => {
  ElMessage({
    showClose: true,
    message: "暂未开通",
    duration: 1000,
  });
};
</script>

<style scoped lang="scss">
.name {
  background-color: red;
}
.vipContainer {
  .header {
    width: 100%;
    height: 15rem;
    background: #c9d6ff;
    background: -webkit-linear-gradient(
      to right,
      rgb(201, 214, 255),
      rgb(168, 188, 255)
    );
    background: linear-gradient(
      to right,
      rgb(201, 214, 255),
      rgb(181, 198, 254)
    );
    display: flex;
    .left {
      flex: 1;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      .head {
        width: 4rem;
        position: absolute;
        right: -3rem;
        top: -2rem;
        transform: rotate(44deg);
      }
      .vip_grade {
        margin-top: 1rem;
        color: rgb(255, 251, 0);
        font-family: "Times New Roman", Times, serif, Arial, sans-serif;
        font-weight: 700;
        font-size: 1.2rem;
      }
      .img {
        border: 1px solid white;
        animation: ani 2s linear infinite;
        border-radius: 100%;
      }
      @keyframes ani {
        0% {
          box-shadow: 0 0 0px #eeb422;
        }
        25% {
          box-shadow: 0 0 10px #eeb422;
        }
        35% {
          box-shadow: 0 0 20px #eeb422;
        }
        50% {
          box-shadow: 0 0 30px #eeb422;
        }
        75% {
          box-shadow: 0 0 20px #eeb422;
        }
        100% {
          box-shadow: 0 0 0px #ffff00;
        }
      }
    }
    .center {
      flex: 1;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: space-around;
      .title {
        min-width: 12rem;
        text-align: center;
        font-size: 1.4rem;
        font-family: "Times New Roman", Times, serif, Arial, sans-serif;
        color: rgb(255, 0, 55);
        border: 1px solid white;
        padding: 1rem;
        border-radius: 10rem;
        display: flex;
        align-items: center;
        justify-content: center;
      }
      .content {
        font-family: "Times New Roman", Times, serif, Arial, sans-serif;
        border: 1px solid white;
        padding: 1rem;
        color: red;
        border-radius: 1rem;
        background: #ffefba; /* fallback for old browsers */
        background: -webkit-linear-gradient(
          to right,
          rgb(255, 239, 186),
          rgb(255, 255, 255)
        ); /* Chrome 10-25, Safari 5.1-6 */
        background: linear-gradient(
          to right,
          rgb(255, 239, 186),
          rgb(255, 255, 255)
        ); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */
        div {
          padding: 0.5rem;
        }
      }
    }
    .right {
      flex: 1;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: space-around;
      .toContinue {
        width: 50%;
        min-width: 10rem;
        color: rgb(255, 0, 123);
        button {
          border: none;
          width: 100%;
          border-radius: 1rem;
          height: 3rem;
          font-size: 1.3rem;
        }
        button:hover {
          cursor: pointer;
          background-color: #fff;
          color: rgb(255, 197, 0);
        }
      }
    }
  }
  .table {
    width: auto;
  }
}
</style>
