<template>
  <div class="box">
    <!-- 搜索 -->
    <div class="search-box">
      <el-input
        v-model="input"
        style="width: 40%; height: 42px"
        size="large"
        placeholder="输入用户ID筛选已同意的设备列表"
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
      <el-button
        v-if="length"
        @click="btnDel"
        style="height: 40px"
        type="danger"
        >批量删除</el-button
      >
    </div>
    <!-- 列表 -->
    <div class="elm-box">
      <el-table
        :data="tableData"
        style="width: 100%; margin-top: -10px"
        @row-click="selectdevicesId"
        @selection-change="handleSelectionChange"
      >
        <el-table-column fixed="left" type="selection" width="55" />
        <el-table-column
          prop="devicesId"
          label="设备ID"
          show-overflow-tooltip
        />
        <el-table-column
          prop="devicesName"
          label="设备名"
          show-overflow-tooltip
        />
        <el-table-column
          prop="userId"
          label="所属用户id"
          show-overflow-tooltip
        />
        <el-table-column prop="secretKey" label="秘钥" show-overflow-tooltip />
        <el-table-column fixed="right" label="操作" width="100">
          <template #default>
            <el-popconfirm
              width="220"
              confirm-button-text="确定"
              cancel-button-text="不了"
              title="你确定要删除此用户吗?"
              @confirm="handleConfirm"
            >
              <template #reference>
                <el-button type="danger" size="small">删除</el-button>
              </template>
            </el-popconfirm>
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
import {
  deviceListApi,
  searchUserListApi,
  deleteDeviceApi,
} from "@/api/devices";
import { ElMessage } from "element-plus";
import { ref, onBeforeMount } from "vue";
let tableData = ref([
  {
    devicesId: "",
    devicesName: "",
    userId: "",
    secretKey: "",
  },
]);
let total = ref(); // 总页数
let pageSize = ref(10); //每页显示多少条数据
let currentPage = ref(1); // 当前在第几页
const input = ref("");
const selectedDevicesId = ref<any[]>([]);
const length = ref(false);
// 分页获取已同意的设备列表
async function fetchdeviceData(_page: number) {
  try {
    const response = await deviceListApi({
      page: currentPage.value,
      pageSize: pageSize.value,
    });
    // console.log(response.data);
    if (response.status == 1) {
      tableData.value = response.data.list;
      total.value = response.data.total * 10;
      // console.log(total.value);
    } else {
      console.error("API返回数据格式不符预期");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
// 挂载之前获取数据
onBeforeMount(() => {
  fetchdeviceData(currentPage.value); // 首次加载第一页数据
});
// 当前页数变化就再加载一次
const handlePageChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchdeviceData(newPage);
};

// 根据用户ID筛选已同意的设备列表
async function searchUserData() {
  try {
    const response = await searchUserListApi({
      userId: input.value,
    });
    // console.log(response);
    if (response.status == 1) {
      tableData.value = response.data.list;
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
// 点击获取选中的 devicesId 数组
const handleSelectionChange = (val: any[]) => {
  // 数组获取选择的每行id
  selectedDevicesId.value = val.map((item: any) => item.devicesId);
  if (selectedDevicesId.value.length > 1) {
    length.value = true;
  } else {
    length.value = false;
  }
  // console.log(selectedDevicesId.value)
  // console.log(selectedDevicesId.value.length);
};
// 点击获取本行id
const selectdevicesId = (row: any) => {
  if ((selectedDevicesId.value = [])) {
    selectedDevicesId.value.push(row.devicesId);
  }
};
// 删除用户信息
async function deleteDevicesData() {
  try {
    const response = await deleteDeviceApi({
      devicesIds: selectedDevicesId.value,
    });
    // console.log(response);
    if (response.status == 1) {
      fetchdeviceData(currentPage.value); //刷新表格数据
      ElMessage({
        message: "删除成功",
        type: "success",
      });
    } else {
      ElMessage.error("删除失败");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
const handleConfirm = () => {
  deleteDevicesData();
};
const btnDel = () => {
  deleteDevicesData();
};
</script>

<style scoped lang="scss">
.box {
  background: white;
  .search-box {
    margin-bottom: 15px;
    padding: 15px 20px 0px;
  }
  .elm-box {
    padding: 15px 20px 15px;
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
