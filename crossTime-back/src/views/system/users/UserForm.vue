<template>
    <el-form ref="form" :model="form" label-width="80px">
        <el-form-item label="用户名">
            <el-input v-model="form.name"></el-input>
        </el-form-item>
        <el-form-item label="性别">
            <el-radio-group v-model="form.sex">
                <el-radio label="男"></el-radio>
                <el-radio label="女"></el-radio>
            </el-radio-group>
        </el-form-item>
        <el-form-item label="地址">
            <el-input v-model="form.address"></el-input>
        </el-form-item>
        <el-form-item label="手机号">
            <el-input placeholder="请输入手机号" maxlength="10" type="number" v-model="form.phone"></el-input>
        </el-form-item>
        <el-form-item label="余额">
            <el-input type="number" placeholder="请输入余额" v-model="form.surplus"></el-input>
        </el-form-item>
        <el-form-item label="VIP状态">
            <el-switch v-model="form.status" active-text="启用" inactive-text="禁用" active-color="#13ce66"
                inactive-color="#ff4949">
            </el-switch>
        </el-form-item>
        <el-form-item label="VIP等级">
            <el-rate style="margin-top: 10px;" :texts="texts" show-text @change="vipGradeEvent" v-model="form.VipGrade"
                text-color="red" :colors="{ 1: '#97FFFF', 2: '#63B8FF', 3: '	#FFDEAD', 4: '#FFB90F', 5: '#FF6A6A' }">
            </el-rate>
        </el-form-item>
        <el-form-item label="累计充值">
            <el-input type="number" placeholder="请输入累计充值金额" v-model="form.totalAcc"></el-input>
        </el-form-item>
        <el-form-item>
            <!-- 子给父传递数据，告知父组件要关闭对话框 -->
            <el-button type="primary" @click="onSubmit">立即创建</el-button>
            <el-button @click="onCancel">取消</el-button>
        </el-form-item>
    </el-form>
</template>

<script>

export default {
    data() {
        return {
            texts: ["svip1", "svip2", "svip3", "svip4", "svip5"],
            form: {
                name: '',
                sex: "",
                address: "",
                phone: "",
                surplus: 0,
                status: false,
                VipGrade: 0,
                totalAcc: 0,
            }
        }
    },
    props: ['FatherMsg'],
    methods: {
        //vip等级
        vipGradeEvent(e) {
            console.log(e);
        },
        //立即创建
        onSubmit() {
            this.$emit('showEvent', false)
            console.log('submit!');
        },
        //取消
        onCancel() {
            this.$emit('showEvent', false)
        }
    }
}
</script>

<style scoped></style>