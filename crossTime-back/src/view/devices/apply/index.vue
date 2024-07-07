<template>
  <div class="box">
    <p
      style="font-size: 20px; padding: 20px 0 10px 30px; display: inline-block"
    >
      <i>待处理的申请设备</i>
    </p>
    <el-button @click="btnApr" class="right" type="danger" v-if="length">批量拒绝</el-button>
    <el-button @click="btnDelrej" class="right" type="success" v-if="length">批量同意</el-button>
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
        <el-table-column
          prop="applyTime"
          label="申请时间"
          show-overflow-tooltip
        />
        <el-table-column fixed="right" label="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;操作" width="140">
          <template #default>
            <el-popconfirm
              width="220"
              confirm-button-text="确定"
              cancel-button-text="不了"
              title="你确定要同意吗?"
              @confirm="handleConfirmApprove"
            >
              <template #reference>
                <el-button type="success" size="small">同意</el-button>
              </template>
            </el-popconfirm>
            <el-popconfirm
              width="220"
              confirm-button-text="确定"
              cancel-button-text="不了"
              title="你确定要拒绝吗?"
              @confirm="handleConfirmReject"
            >
              <template #reference>
                <el-button type="danger" size="small">拒绝</el-button>
              </template>
            </el-popconfirm>
          </template>
        </el-table-column>
      </el-table>
    </div>
  </div>
</template>

<script setup lang="ts">
import { applyDeviceListApi, deviceDecisionApi } from "@/api/devices";
import { ElMessage } from "element-plus";
import { onBeforeMount, ref } from "vue";
let tableData = ref([
  {
    devicesId: "",
    devicesName: "",
    userId: "",
    secretKey: "",
    applyTime: "",
  },
]);
let selecteddevicesId = ref();
let decision = ref();
const length = ref(false);
async function applydeviceData() {
  try {
    const response = await applyDeviceListApi();
    // console.log(response.data);
    if (response.status == 1) {
      tableData.value = response.data.list;
    } else {
      console.error("API返回数据格式不符预期");
    }
  } catch (error) {
    console.error("请求失败:", error);
  }
}
// 挂载之前获取数据
onBeforeMount(() => {
  applydeviceData();
});
// 点击获取本行devicesId
const selectdevicesId = (row: any) => {
  if ((selecteddevicesId.value = [])) {
    selecteddevicesId.value.push(row.devicesId);
    // console.log(selecteddevicesId.value);
  }
};
// 点击获取选中的 devicesId 数组
const handleSelectionChange = (val: any[]) => {
  // 数组获取选择的每行id
  selecteddevicesId.value = val.map((item: any) => item.devicesId);
  if (selecteddevicesId.value.length > 1) {
    length.value = true;
  } else {
    length.value = false;
  }
  // console.log(selectedDevicesId.value)
  // console.log(selectedDevicesId.value.length);
};
// 处理申请设备
async function deviceDecisionData() {
  try {
    const response = await deviceDecisionApi({
      devicesId: selecteddevicesId.value,
      type: decision.value,
    });
    // console.log(response);
    if (response.status == 1) {
      applydeviceData(); //刷新表格数据
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
const handleConfirmApprove = () => {
  decision.value = "approve";
  // console.log(decision.value)
  deviceDecisionData();
};
const handleConfirmReject = () => {
  decision.value = "reject";
  // console.log(decision.value)
  deviceDecisionData();
};
const btnApr = () => {
  decision.value = "approve";
  deviceDecisionData();
};
const btnDelrej = () => {
  decision.value = "reject";
  deviceDecisionData();
};
</script>

<style scoped lang="scss">
.box {
  background: white;
  .right {
    height: 35px;
    margin: 20px 15px 0 0;
    float: right;
    line-height: 35px;
  }
  .elm-box {
    padding: 20px;
  }
}
</style>
