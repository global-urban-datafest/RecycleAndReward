# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('traceapp', '0004_auto_20150307_0726'),
    ]

    operations = [
        migrations.AddField(
            model_name='bag',
            name='points',
            field=models.IntegerField(default=0),
            preserve_default=True,
        ),
    ]
