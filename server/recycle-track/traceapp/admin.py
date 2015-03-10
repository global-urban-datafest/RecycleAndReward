from django.contrib import admin
from django.utils.translation import ugettext as _
from django.contrib.auth.admin import UserAdmin
from django.contrib.auth.models import User

from traceapp.models import *

# Register your models here.
class GarbageTraceProfileAdmin(admin.ModelAdmin):
    list_display = ['user_display', 'points', 'points_onway', 'points_failed', 'address', 'total_weight']    

    def user_display(self, obj):
        return obj.user.get_full_name()

class BagAdmin(admin.ModelAdmin):
    list_display = ['barcode', 'user', 'points', 'weight', 'status', 'bag_type']

class CheckPointAdmin(admin.ModelAdmin):
    list_display = ['name', 'key', 'point_type']

class BagTraceAdmin(admin.ModelAdmin):
    list_display = ['user_display', 'bag', 'check_point', 'event', 'time', 'extra']

    def user_display(self, obj):
        return obj.user.get_full_name()

admin.site.register(GarbageTraceProfile, GarbageTraceProfileAdmin)
admin.site.register(Bag, BagAdmin)
admin.site.register(CheckPoint, CheckPointAdmin)
admin.site.register(BagTrace, BagTraceAdmin)

