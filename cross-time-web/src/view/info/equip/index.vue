<template>
  <div class="box">
    <!-- 列表 -->
    <div class="elm-box">
      <el-table align="center" :data="tableData">
        <el-table-column align="center" prop="name" label="设备名" />
        <el-table-column align="center" prop="ip" label="设备IP" />
        <el-table-column prop="eStatus" label="设备状况">
          <template #default="scope">
            <el-tag :type="scope.row.eStatus ? 'success' : 'danger'">
              {{ scope.row.eStatus ? "正常" : "异常" }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column align="center" fixed="right" label="操作">
          <template #default="scope">
            <el-button
              type="primary"
              size="small"
              @click="updateData(scope.row, scope.$index)"
            >
              更改
            </el-button>
            <el-popconfirm
              @cancel="CancelDeltete"
              @confirm="ConfirmDelete(scope.$index)"
              title="确定删除吗？"
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
        small
        background
        layout="prev, pager, next"
        :total="1"
        class="mt-4"
      />
    </div>
    <el-dialog v-model="isDialog" width="500">
      <div style="margin-bottom: 1rem">请输入要更改的设备名：</div>
      <el-input v-model="inputValue" placeholder="请输入要更改的设备名：" />
      <template #footer>
        <div class="dialog-footer">
          <el-button @click="isDialog = false">取消</el-button>
          <el-button type="primary" @click="confirm">确定</el-button>
        </div>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from "vue";
import UseHomeStore from "@/store/modules/home/home";
import { ElMessage } from "element-plus";
const homeStore = UseHomeStore();
let tableData: any = ref([]);
let isDialog = ref(false);
let inputValue = ref("");
let updateIndex = -1;
//点击更改
const updateData = (item: any, index: any) => {
  isDialog.value = true;
  console.log(item, index);
  updateIndex = index;
};
//确定更改
const confirm = () => {
  homeStore.equipInfo.equipments[updateIndex].name = inputValue.value;
  isDialog.value = false;
};
//确定删除
const ConfirmDelete = (index: any) => {
  tableData.value.splice(index, 1);
  ElMessage({
    type: "success",
    message: "删除成功",
  });
};
//取消删除
const CancelDeltete = () => {};
onMounted(async () => {
  await homeStore.init();
  // console.log(homeStore.equipInfo.equipments);
  // homeStore.equipInfo.equipments.map((item) => {
  //   tableData.value.push({});
  // });{
  if (homeStore.equipInfo.equipments) {
    tableData.value = [...homeStore.equipInfo.equipments];
  }
  console.log(tableData.value);
});
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
    .el-pagination {
      float: right;
      padding-right: 20px;
    }
  }
}
</style>
