# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('traceapp', '0002_auto_20150307_0610'),
    ]

    operations = [
        migrations.AddField(
            model_name='bag',
            name='status',
            field=models.IntegerField(default=0, choices=[(0, 'Unassigned'), (1, 'Assigned'), (2, 'Used'), (3, 'Transfering'), (4, 'Recycled')]),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='bag',
            name='weight',
            field=models.FloatField(default=0),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='bagtrace',
            name='extra',
            field=models.CharField(max_length=50, null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='bagtrace',
            name='time',
            field=models.DateTimeField(auto_now_add=True),
            preserve_default=True,
        ),
    ]
