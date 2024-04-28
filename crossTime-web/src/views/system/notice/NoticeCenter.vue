<template>
  <div class="noticeContainer">
    <div v-if="isNotify" class="container">
      <el-table :data="notifications">
        <el-table-column prop="title" label="标题" width="290"></el-table-column>
        <el-table-column prop="message" label="内容" width="auto"
          style="overflow: hidden;text-overflow:ellipsis;white-space: nowrap;"></el-table-column>
        <el-table-column prop="createdAt" label="创建时间" width="180"></el-table-column>
      </el-table>
    </div>
    <div v-else>
      <el-empty description="暂无通知"
        style="position: absolute;left: 55%;top: 50%;transform: translate(-50%,-50%);"></el-empty>
    </div>
  </div>
</template>

<script>
import { get } from '@/plugins/request';
export default {
  data() {
    return {
      isNotify: true,
      notifications: [],
    };
  },
  methods: {
    async init() {
      let res = await get("/getAllNotice");
      if (res.total == 0) {
        this.isNotify = false;
      }
      else {
        this.notifications = [...res.notice];

      }
    }
  },
  mounted() {
    this.init();
  }
};
</script>
<style lang="css" scoped>
.container {
  caret-color: rgba(0, 0, 0, 0);
  width: 70vw;
  display: flex;
  justify-content: center;
  align-content: center;
  padding: 3%;
  /* background-color: rebeccapurple; */
  min-width: 50rem;
}

.noticeContainer {
  transform: translate(10%);
  min-width: 50rem;
  position: absolute;
}
</style>
