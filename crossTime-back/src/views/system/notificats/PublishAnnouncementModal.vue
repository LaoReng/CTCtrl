<template>
  <el-dialog title="发布公告" :visible.sync="visible" @close="onClose">
    <el-form ref="form" :model="form" label-width="100px">
      <el-form-item label="管理员ID">
        <el-input
          v-model="form.adminId"
          placeholder="请输入管理员ID"
          disabled
        ></el-input>
      </el-form-item>
      <el-form-item label="内容">
        <el-input
          type="textarea"
          v-model="form.content"
          autosize
          placeholder="请输入公告内容"
        ></el-input>
      </el-form-item>
    </el-form>
    <div slot="footer" class="dialog-footer">
      <el-button @click="onClose">取消</el-button>
      <el-button type="primary" @click="publish">发布</el-button>
    </div>
  </el-dialog>
</template>

<script>
export default {
  props: {
    visible: Boolean,
  },
  data() {
    return {
      form: {
        adminId: "",
        content: "",
      },
    };
  },
  methods: {
    onClose() {
      this.$emit("update:visible", false);
    },
    publish() {
      // 调用API发布公告的逻辑，成功后关闭模态框
    },
  },
};
</script>

<style scoped></style>
