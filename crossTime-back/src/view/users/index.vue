<template>
  <div class="box">
    <!-- 搜索 -->
    <div class="search-box">
      <el-input
        v-model="input"
        style="width: 40%; height: 42px"
        size="large"
        placeholder="输入地址筛选用户信息"
      />
      <button
        @click="btnSearch"
        style="margin-left: 10px; height: 40px"
        aria-disabled="false"
        type="button"
        class="el-button el-button--primary"
      >
        <span
          ><i
            class="el-icon"
            style="vertical-align: middle; color: rgb(255, 255, 255)"
            ><svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024">
              <path
                fill="currentColor"
                d="m795.904 750.72 124.992 124.928a32 32 0 0 1-45.248 45.248L750.656 795.904a416 416 0 1 1 45.248-45.248zM480 832a352 352 0 1 0 0-704 352 352 0 0 0 0 704"
              ></path></svg></i
          ><span style="vertical-align: middle"> 搜索 </span></span
        >
      </button>
      <el-button v-if="length" @click="btnApr" class="right" type="danger"
        >批量禁用</el-button
      >
      <el-button v-if="length" @click="btnDelrej" class="right" type="success"
        >批量启用</el-button
      >
    </div>
    <!-- 列表 -->
    <div class="elm-box">
      <el-table
        :data="tableData"
        style="width: 100%; margin-top: -10px"
        @selection-change="handleSelectionChange"
        @row-click="selectUser"
      >
        <el-table-column fixed="left" type="selection" width="55" />
        <el-table-column
          prop="userId"
          label="用户ID"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="username"
          label="用户名"
          width="125"
          show-overflow-tooltip
        />
        <el-table-column
          prop="sex"
          label="性别"
          width="80"
          show-overflow-tooltip
        />
        <el-table-column
          prop="address"
          label="地址"
          width="200"
          show-overflow-tooltip
        />
        <el-table-column
          prop="phone"
          label="电话"
          width="125"
          show-overflow-tooltip
        />
        <el-table-column
          prop="registerTime"
          label="注册时间"
          width="170"
          show-overflow-tooltip
        />
        <el-table-column
          prop="expire"
          label="是否停用"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="vipStatus"
          label="VIP状态"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="vipLevel"
          label="VIP等级"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="expirationDate"
          label="VIP到期时间"
          width="170"
          show-overflow-tooltip
        />
        <el-table-column
          prop="balance"
          label="余额"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="totalRecharge"
          label="累计充值"
          width="100"
          show-overflow-tooltip
        />
        <el-table-column
          prop="deviceControlTime"
          label="可控天数"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="controlCount"
          label="可控次数"
          width="90"
          show-overflow-tooltip
        />
        <el-table-column
          prop="remainingCount"
          label="剩余次数"
          width="90"
          show-overflow-tooltip
        />

        <el-table-column fixed="right" label="操作" width="75">
          <template #default="scope">
            <template v-if="scope.row.forbidden">
              <!-- 启用用户的逻辑 -->
              <el-popconfirm
                confirm-button-text="确定"
                cancel-button-text="不了"
                title="你确定要启用此用户吗?"
                @confirm="handledis"
              >
                <template #reference>
                  <el-button type="success" size="small">启用</el-button>
                </template>
              </el-popconfirm>
            </template>
            <template v-else>
              <!-- 禁用用户的逻辑 -->
              <el-popconfirm
                confirm-button-text="确定"
                cancel-button-text="不了"
                title="你确定要禁用此用户吗?"
                @confirm="handable"
              >
                <template #reference>
                  <el-button type="danger" size="small">禁用</el-button>
                </template>
              </el-popconfirm>
            </template>
          </template>
        </el-table-column>
      </el-table>
    </div>
    <!-- 分页 -->
    <div class="page-box">
      <el-pagination
        class="page"
        :page-size="pageSize"
        :current-page="currentPage"
        layout="prev, pager, next, jumper"
        :total="total"
        @current-change="handlePageChange"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { userListApi, searchUserListApi, deleteUserApi } from "@/api/users";
import { ElMessage } from "element-plus";
import { ref, onBeforeMount } from "vue";
let tableData = ref([
  {
    userId: "",
    username: "",
    sex: "",
    address: "",
    phone: "",
    registerTime: "",
    vipStatus: "",
    vipLevel: "",
    expirationDate: "",
    balance: "",
    totalRecharge: "",
    deviceControlTime: "",
    controlCount: "",
    remainingCount: "",
    expire: "", //停用
    forbidden: "", //禁用启用
  },
]);
// const getRowClass = ({ row }: { row: any }) => {
//   //  console.log(row)
//   return row.expire ? "expired-row" : "";
// };
// console.log(tableData.value);

let total = ref(); // 总页数
let pageSize = ref(10); //每页显示多少条数据
let currentPage = ref(1); // 当前在第几页
let input = ref(""); //搜索框
let state = ref(""); //删除/启用/禁用
const length = ref(false);
const selectedUserId = ref<any[]>([]);

// 分页获取用户信息
async function fetchUserData(_page: number) {
  try {
    const response = await userListApi({
      page: currentPage.value,
      pageSize: pageSize.value,
    });
    // console.log(response.data);
    if (response.status == 1) {
      tableData.value = response.data.list;
      total.value = response.data.total * 10;
      // console.log(total.value);
      tableData.value.forEach((UserData) => {
        UserData.sex = UserData.sex == "1" ? "女" : "男";
        UserData.vipStatus = UserData.vipStatus == "1" ? "激活" : "未激活";
      });
      tableData.value.forEach((UserData) => {
        UserData.sex = UserData.sex == "1" ? "女" : "男";
        UserData.vipStatus = UserData.vipStatus == "1" ? "激活" : "未激活";
        UserData.expire = UserData.expire ? "是" : "否";
      });
    } else {
      console.error("API返回数据格式不符预期");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
// 挂载之前获取数据
onBeforeMount(() => {
  fetchUserData(currentPage.value); // 首次加载第一页数据
  // tableData.value.forEach((row) => {
  //   console.log(row.expire); // 输出每一行的数据
  // });
});
// 当前页数变化就再加载一次
const handlePageChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchUserData(newPage);
};

// 根据地址筛选用户信息
async function searchUserData() {
  try {
    const response = await searchUserListApi({
      address: input.value,
    });
    // console.log(response);
    if (response.status == 1) {
      tableData.value = response.data.list;
      tableData.value.forEach((UserData) => {
        UserData.sex = UserData.sex == "1" ? "女" : "男";
        UserData.vipStatus = UserData.vipStatus == "1" ? "激活" : "未激活";
        UserData.expire = UserData.expire ? "是" : "否";
      });
    } else {
      console.error("API返回数据格式不符预期");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const btnSearch = () => {
  searchUserData();
};

// 点击获取本行userId数组
const handleSelectionChange = (val: any[]) => {
  // 数组获取选择的每行id
  selectedUserId.value = val.map((item: any) => item.userId);
  if (selectedUserId.value.length > 1) {
    length.value = true;
  } else {
    length.value = false;
  }
  // console.log(selectedUserId.value)
  // console.log(selectedUserId.value.length);
};
// 点击获取本行id
const selectUser = (row: any) => {
  // selectedUserId.value = row.userId;
  if ((selectedUserId.value = [])) {
    selectedUserId.value.push(row.userId);
  }
  // console.log(selectedUserId.value);
  // console.log(row.forbidden);
};

// 删除用户信息
async function deleteUserData() {
  try {
    const response = await deleteUserApi({
      userIds: selectedUserId.value,
      type: state.value,
    });
    console.log(response);
    if (response.status == 1) {
      fetchUserData(currentPage.value); //刷新表格数据
      ElMessage({
        message: "操作成功",
        type: "success",
      });
    } else {
      ElMessage.error("操作失败");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const handledis = () => {
  state.value = "enabled";
  deleteUserData();
};
const handable = () => {
  state.value = "disabled";
  deleteUserData();
};
const btnApr = () => {
  state.value = "disabled";
  deleteUserData();
};
const btnDelrej = () => {
  state.value = "enabled";
  deleteUserData();
};
</script>

<style scoped lang="scss">
.box {
  background: white;
  .search-box {
    margin-bottom: 15px;
    padding: 15px 20px 0px;
  }
  .right {
    height: 40px;
    margin: 0 15px 0 0;
    float: right;
    line-height: 40px;
  }
  .elm-box {
    padding: 15px 20px 15px;
    // .el-table__row expired-row {
    //   background-color: #f0f0f0;
    //   color: red;
    //   --el-table-tr-bg-color: #f0f0f0;
    // }
  }
  .page-box {
    height: 3rem;
    background: white;
    .page {
      float: right;
      padding-right: 20px;
    }
  }
}
</style>
