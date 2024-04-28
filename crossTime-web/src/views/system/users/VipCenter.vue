<template>
  <div style="display: flex;align-items: center;justify-content: center;height: calc(100vh - 60px)" >
    <div class="vip-center-container">
      <div class="vip-header">
        <div class="vip-info">
          <h3>{{ vip.name }}</h3>
          <p>当前VIP等级：{{ vip.levelName }}</p>
        </div>
      </div>

      <div class="vip-status">
        <p>
          有效期至：<span class="expiration-date">{{ vip.expirationDate }}</span>
        </p>
      </div>

      <h2 class="section-title">特权列表</h2>
      <ul class="privilege-list">
        <li v-for="privilege in privileges" :key="privilege.id">
          <i class="el-icon-check"></i>
          {{ privilege.title }}
        </li>
      </ul>

      <div class="action-buttons">
        <el-button type="primary" @click="openRechargeModal">充值</el-button>
        <el-button type="success" @click="openUpgradeModal">升级</el-button>
      </div>

      <el-dialog :visible.sync="rechargeModalVisible" title="充值">
        <el-form ref="rechargeForm" :model="rechargeFormData" label-width="80px">
          <el-form-item label="充值金额">
            <el-input-number v-model="rechargeFormData.amount" :min="1" :controls="false"></el-input-number>
          </el-form-item>
          <div slot="footer" class="dialog-footer">
            <el-button @click="rechargeModalVisible = false">取消</el-button>
            <el-button type="primary" @click="submitRecharge">确认充值</el-button>
          </div>
        </el-form>
      </el-dialog>

      <el-dialog :visible.sync="upgradeModalVisible" @close="clearUpgradeForm" title="升级">
        <el-form ref="upgradeForm" :model="upgradeFormData" label-width="80px">
          <el-form-item label="选择新等级">
            <el-select v-model="upgradeFormData.newLevel" placeholder="请选择新等级">
              <el-option v-for="level in upgradeLevels" :key="level.id" :label="level.name"
                :value="level.id"></el-option>
            </el-select>
          </el-form-item>
          <div slot="footer" class="dialog-footer">
            <el-button @click="upgradeModalVisible = false">取消</el-button>
            <el-button type="primary" @click="submitUpgrade">确认升级</el-button>
          </div>
        </el-form>
      </el-dialog>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      vip: {
        name: "VIP会员",
        levelName: "白银会员",
        expirationDate: "2024-05-21",
      },
      privileges: [
        { id: 1, title: "增加控制时长" },
        { id: 2, title: "增加控制次数" },
      ],
      rechargeModalVisible: false,
      rechargeFormData: {
        amount: 0,
      },
      upgradeModalVisible: false,
      upgradeFormData: {
        newLevel: "",
      },
      upgradeLevels: [
        { id: 1, name: "黄金会员" },
        { id: 2, name: "钻石会员" },
      ],
    };
  },
  methods: {
    openRechargeModal() {
      this.$message({
        message: '当前服务暂未开通',
        type: 'warning',
        showClose: "true",
        duration: 1000
      });
    },
    openUpgradeModal() {
      this.$message({
        message: '当前服务暂未开通',
        type: 'warning',
        showClose: "true",
        duration: 1000
      });
    },
    submitUpgrade() {

    },
    clearRechargeForm() {

    },
    submitRecharge() {

    },
    clearUpgradeForm() {

    }

  }
};
</script>

<style scoped>
.vip-center-container {
  display: flex;
  padding: 1rem;
  width: calc(100vw - 10rem);
  flex-direction: column;
  align-items: center;
  justify-content: center;
  /* height: 85vh; */
}

.vip-header {
  display: flex;
  align-items: center;
  margin-bottom: 30px;
}

.vip-avatar {
  width: 100px;
  height: 100px;
  border-radius: 50%;
  object-fit: cover;
  margin-right: 20px;
}

.vip-info h3 {
  text-align: center;
  font-size: 24px;
  font-weight: bold;
  margin-bottom: ¼px;
}

.vip-status {
  text-align: center;
  margin-bottom: 30px;
}

.section-title {
  text-align: center;
  font-size: 24px;
  font-weight: bold;
  margin-bottom: 20px;
}

.privilege-list {
  list-style: none;
  padding: 0;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 20px;
}

.privilege-list li {
  display: flex;
  align-items: center;
  flex-basis: calc(33.33% - ⅔px);
}

.privilege-list i {
  font-size: 24px;
  color: #409eff;
  margin-right: 10px;
}

.action-buttons {
  display: flex;
  justify-content: center;
  margin-top: 25px;
}
</style>
