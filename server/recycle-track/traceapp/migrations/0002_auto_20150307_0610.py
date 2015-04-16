# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        ('traceapp', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='checkpoint',
            name='address',
            field=models.CharField(default=b'', max_length=100),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='bag',
            name='user',
            field=models.ForeignKey(related_name='BagUser', default=None, blank=True, to=settings.AUTH_USER_MODEL, null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='checkpoint',
            name='point_type',
            field=models.IntegerField(choices=[(0, 'Dustbin'), (1, 'Disctrict Center'), (2, 'Recycling Center')]),
            preserve_default=True,
        ),
    ]
