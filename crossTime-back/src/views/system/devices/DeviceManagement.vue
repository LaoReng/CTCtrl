<template>
  <div class="device-management">
    <h1 class="page-title">设备管理</h1>
    <el-card style="border-radius: 15px" shadow="always">
      <el-table :data="devices" style="width: 100%">
        <el-table-column prop="id" label="设备ID"></el-table-column>
        <el-table-column
          prop="name"
          label="设备名"
          show-overflow-tooltip
        ></el-table-column>
        <el-table-column prop="userId" label="所属用户ID"></el-table-column>
        <el-table-column
          prop="secretKey"
          label="密钥"
          show-overflow-tooltip
        ></el-table-column>
        <el-table-column prop="applyTime" label="申请时间"></el-table-column>
        <el-table-column
          prop="status"
          label="申请状态"
          width="76"
        ></el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      devices: [
        {
          id: "admin-001",
          name: "Smart Device A",
          userId: "user-001",
          secretKey: "1234567890ABCDEF",
          applyTime: "2024-04-05 20:13:14",
          status: "在线",
        },
      ],
    };
  },
  // async created() {
  //   try {
  //     const response = await axios.get("http://your-api-url/devices");
  //     this.devices = response.data;
  //   } catch (error) {
  //     console.error("Failed to fetch devices:", error);
  //   }
  // },
};
</script>

<style scoped>
.device-management {
  padding: 20px;
}
</style>
