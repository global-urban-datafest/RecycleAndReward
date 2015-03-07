from django.db import models
from django.contrib.auth.models import User
from django.utils.translation import ugettext as _
from django.db.models.signals import post_save

# Create your models here.
class GarbageConstants(object):
    BAG_MAX_WEIGHT = 100

class GarbageTraceProfile(models.Model):
    # This field is required.
    user = models.OneToOneField(User, related_name='profile')
    address = models.CharField(_("Address"), max_length=255, default='')    

    points = models.IntegerField(_("Points"), default=0)
    points_onway = models.IntegerField(_("Points on the way"), default=0)
    points_failed = models.IntegerField(_("Points failed"), default=0)

    total_weight = models.FloatField(_("Total Weight"), default=0)

# Attached profile creation upon user creation
def create_user_profile(sender, instance, created, **kwargs):
    if created:
        GarbageTraceProfile.objects.create(user=instance)

post_save.connect(create_user_profile, sender=User)


class Bag(models.Model):
    BAG_STATUS = (
        (0, _('Unassigned')),
        (1, _('Assigned')),
        (2, _('Used')),
        (3, _('Transfering')),
        (4, _('Recycled')),
    )
    BAG_TYPE = (
        (0, _('General')),
    )
        
    barcode = models.CharField(max_length=50)
    user = models.ForeignKey(User, related_name="BagUser", null=True, blank=True, default=None)    
    weight = models.FloatField(default=0)
    status = models.IntegerField(choices=BAG_STATUS, default=0)
    bag_type = models.IntegerField(choices=BAG_TYPE, default=0)

    def __unicode__(self):
        return self.barcode

class CheckPoint(models.Model):
    CHECK_POINT_TYPE = (
        (0, _("Dustbin")),
        (1, _("Disctrict Center")),
        (2, _("Recycling Center")),
    )
    name = models.CharField(max_length=100)
    key = models.CharField(max_length=255)
    point_type = models.IntegerField(choices=CHECK_POINT_TYPE)
    address = models.CharField(max_length=100, default='')

    def __unicode__(self):
        return self.name

class BagTrace(models.Model):
    BAG_TRACE_EVENT = (
        (0, _("Assign to user")),
        (1, _("Dump bag")),
        (2, _("Bag accepted")),
        (3, _("Bag rejected")),
        (4, _("Bag arrived")),
        (5, _("Bag passed")),
        (6, _("Bag failed")),
    )

    user = models.ForeignKey(User, related_name="BagTraceUser+")
    bag = models.ForeignKey(Bag, related_name="BagTraceBag+")
    check_point = models.ForeignKey(CheckPoint, related_name="BagTraceCheckPoint+")
    event = models.PositiveIntegerField(choices=BAG_TRACE_EVENT)
    time = models.DateTimeField(auto_now_add=True)
    extra = models.CharField(max_length=50, null=True, blank=True)



