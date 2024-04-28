<template>
  <div class="announcement-management">
    <div style="overflow: hidden">
      <h1 class="page-title">通知公告管理</h1>
      <div class="top-right">
        <el-button type="primary" @click="showPublishModal">发布公告</el-button>
      </div>
    </div>
    <el-card style="border-radius: 15px" shadow="always" class="nty-box">
      <el-table :data="announcements" style="width: 100%">
        <el-table-column prop="id" label="公告ID"></el-table-column>
        <el-table-column prop="adminId" label="管理员ID"></el-table-column>
        <el-table-column prop="content" label="内容" width="400">
          <template slot-scope="{ row }">
            <span class="ellipsis" :title="row.content">{{ row.content }}</span>
          </template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间"></el-table-column>
        <el-table-column prop="updateTime" label="更新时间"></el-table-column>
        <el-table-column label="操作" width="50">
          <template slot-scope="{ row }">
            <el-button
              type="text"
              size="small"
              @click="deleteAnnouncement(row.id)"
              >删除</el-button
            >
          </template>
        </el-table-column>
      </el-table>
    </el-card>
    <publish-announcement-modal
      :visible.sync="publishModalVisible"
      @close="onPublishModalClose"
    />
  </div>
</template>

<script>
import PublishAnnouncementModal from "./PublishAnnouncementModal.vue";
import axios from "axios";

export default {
  components: {
    PublishAnnouncementModal,
  },
  data() {
    return {
      announcements: [
        {
          id: "announcement-001",
          adminId: "admin-001",
          content:
            "重要通知:系统将于今晚22:00至凌晨2:00进行维护,期间可能导致短暂服务中断，请提前做好准备。",
          createTime: "2024-04-01 10:00:00",
          updateTime: "2024-04-01 10:15:00",
        },
      ],
      publishModalVisible: false,
    };
  },
  // async created() {
  //   try {
  //     const response = await axios.get("http://your-api-url/announcements");
  //     this.announcements = response.data;
  //   } catch (error) {
  //     console.error("Failed to fetch announcements:", error);
  //   }
  // },
  methods: {
    showPublishModal() {
      this.publishModalVisible = true;
    },
    onPublishModalClose() {
      this.publishModalVisible = false;
    },
    // async deleteAnnouncement(id) {
    //   try {
    //     await axios.delete(`http://your-api-url/announcements/${id}`);
    //     this.getAnnouncements();
    //     this.$message.success("公告删除成功");
    //   } catch (error) {
    //     console.error("Failed to delete announcement:", error);
    //     this.$message.error("公告删除失败");
    //   }
    // },
  },
};
</script>

<style scoped>
.announcement-management {
  padding: 20px;
}
.page-title {
  float: left;
}
.top-right {
  float: right;
}
.ellipsis {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
</style>
