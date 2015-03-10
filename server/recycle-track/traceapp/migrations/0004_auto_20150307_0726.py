# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        ('traceapp', '0003_auto_20150307_0653'),
    ]

    operations = [
        migrations.AddField(
            model_name='bag',
            name='bag_type',
            field=models.IntegerField(default=0, choices=[(0, 'General')]),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='garbagetraceprofile',
            name='user',
            field=models.OneToOneField(related_name='profile', to=settings.AUTH_USER_MODEL),
            preserve_default=True,
        ),
    ]
