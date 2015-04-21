# Garbage Tracing / Feedback System


### 积分规则  Points Rules

根据重量进行积分，精确到公斤读数的小数点后两位，1公斤 = 100 点

1.00kg = 100 points

### 服务器接口 Service API

 * 分配垃圾袋 Assign Bags

http://localhost:8000/traceapp/upload/?barcode=1000000001&key=AAAAA&userid=2

 * 丢垃圾 Dump Bags

http://localhost:8000/traceapp/upload/?barcode=1000000001&key=AAAAA&weight=1.1

 * 垃圾转运点 Transfer Bags (action: 4 = passed)

http://localhost:8000/traceapp/checkpoint/?barcode=1000000001&key=AAAAB&action=4

 * 垃圾分拣 Recycle Bags (action: 4 = arrived, 5 = passed, 6 = failed)

http://localhost:8000/traceapp/validate/?barcode=1000000001&key=AAAAB&action=4 

http://localhost:8000/traceapp/validate/?barcode=1000000001&key=AAAAB&action=5

http://localhost:8000/traceapp/validate/?barcode=1000000001&key=AAAAB&action=6


### 后台管理界面 Administration Backend

http://localhost:8000/admin/

 * 看垃圾袋信息 Check Bags Status

http://localhost:8000/admin/traceapp/bag/

 * 看每个人的积分 Check User Status

http://localhost:8000/admin/traceapp/garbagetraceprofile/

 * 看垃圾跟踪情况 Trace Garbage Bag

http://localhost:8000/admin/traceapp/bagtrace/

### 返回代码列表 List of Return Codes

 * 00 = 请求已接受 ACCEPTED
 * 01 = 条形码已被使用 BARCODE_USED
 * 02 = 条形码无效 BARCODE_INVALID
 * 03 = 操作失败 ACTION_FAILED
 * 04 = 请求参数不足 FIELDS_NOT_ENOUGH
 * 05 = 机构无效 CHECKPOINT_INVALID
 * 06 = 重量无效 WEIGHT_INVALID
 * 07 = 用户无效 USER_INVALID

